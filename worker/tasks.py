import os
import subprocess
from celery import Celery

BROKER_URL = os.getenv("CELERY_BROKER_URL", "redis://redis:6379/0")
BACKEND_URL = os.getenv("CELERY_RESULT_BACKEND", "redis://redis:6379/1")

celery = Celery("tasks", broker=BROKER_URL, backend=BACKEND_URL)

@celery.task(name="worker.processor.process_video")
def process_video(input_file, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    result = subprocess.run(["/app/processor", input_file, output_dir], capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(result.stderr)
    return "done"
