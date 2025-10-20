FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    ffmpeg \
    g++ \
    make \
    cmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Install JSON library for C++ (header only)
RUN apt-get update && apt-get install -y nlohmann-json3-dev

# Compile C++ worker
RUN g++ processor.cpp -std=c++17 -o processor

# Default command: wait (Celery worker will call executable)
CMD ["sleep", "infinity"]