import os
import uuid
from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.responses import JSONResponse
from celery import Celery
from pathlib import Path
import subprocess

BROKER_URL = os.getenv("CELERY_BROKER_URL", "redis://redis:6379/0")
BACKEND_URL = os.getenv("CELERY_RESULT_BACKEND", "redis://redis:6379/1")

STORAGE_PATH = Path(os.getenv("STORAGE_PATH", "/data"))

app = FastAPI(title="Video Processing Pipeline API")

celery = Celery(
    "tasks",
    broker=BROKER_URL,
    backend=BACKEND_URL
)

@app.post("/upload")
async def upload_video(file: UploadFile = File(...)):
    if file.content_type.split("/")[0] != "video":
        raise HTTPException(status_code=400, detail="Uploaded file is not a video")

    job_id = str(uuid.uuid4())
    job_dir = STORAGE_PATH / job_id
    job_dir.mkdir(parents=True, exist_ok=True)

    file_path = job_dir / file.filename
    with file_path.open("wb") as f:
        while chunk := await file.read(1024*1024):
            f.write(chunk)

    celery.send_task("worker.processor.process_video", args=[str(file_path), str(job_dir)])

    return JSONResponse({"job_id": job_id, "filename": file.filename})

@app.get("/status/{job_id}")
def status(job_id: str):
    job_dir = STORAGE_PATH / job_id # folder job has its own folder
    if not job_dir.exists():
        raise HTTPException(404, "Job not found")

    status_file = job_dir / "status.json"
    if status_file.exists():
        import json
        return JSONResponse(json.loads(status_file.read_text()))
    else:
        return JSONResponse({"status": "queued_or_processing"})