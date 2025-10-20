# ---------- Stage1 1: Build ----------
FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    wget \
    gpg \
    software-properties-common \
    && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg \
    && echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    && apt-get update && apt-get install -y \
        build-essential \
        cmake \
        git \
        nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -- -j$(nproc)

# ---------- Stage 2: Runtime ----------
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/ /app/

EXPOSE 5149
CMD ["./MyHiveMind"]