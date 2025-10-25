FROM python:3.11-slim

# Install C++ compiler, ffmpeg, and wget
RUN apt-get update && apt-get install -y \
    g++ \
    ffmpeg \
    x264 \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Create nlohmann include path and download header
RUN mkdir -p /usr/local/include/nlohmann
RUN wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -O /usr/local/include/nlohmann/json.hpp

WORKDIR /app

# Copy your C++ processor
COPY processor.cpp .
RUN g++ processor.cpp -std=c++17 -I/usr/local/include -o processor

# Copy Python Celery task code (you’ll need a tasks.py)
COPY tasks.py .

# Install Python requirements
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Run Celery worker
CMD ["celery", "-A", "tasks", "worker", "--loglevel=info"]
