import os
import uuid
from fastapi import FastAPI, UploadFile, File, HTTPException
from fastapi.responses import JSONResponse
from celery import Celery
from pathlib import Path
import subprocess

app = FastAPI()

@app.get("/hello")
def hello():
    return {"message": "Hello, world!"}