# Use the official Python image with PlatformIO
FROM python:3.10-slim

# Set environment variables
ENV IDF_PATH /opt/esp-idf
ENV PATH $IDF_PATH/tools:$PATH

# Install dependencies and tools in a single RUN command to reduce layers
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    wget \
    curl \
    unzip \
    build-essential \
    libffi-dev \
    libssl-dev \
    libncurses-dev \
    libusb-1.0-0 \
    libncurses5 \
    libncursesw5 \
    xz-utils \
    python3-pip \
    cmake \
    ninja-build \
    ccache \
    flex \
    bison \
    gperf \
    qemu-system-x86 \
    qemu-utils \
    libpixman-1-dev \
    libglib2.0-dev \
    pkg-config \
    meson \
    ninja-build \
    dfu-util \
    libgcrypt20-dev \
    ruby \
    socat \
    libslirp-dev && \
    rm -rf /var/lib/apt/lists/*

# Install PlatformIO and esptool using pip
RUN pip install -U platformio esptool

# Clone and install ESP-IDF
WORKDIR /opt
RUN git clone --recursive https://github.com/espressif/esp-idf.git && \
    cd esp-idf && \
    ./install.sh

# Clone and install qemu for ESP32
RUN git clone --quiet https://github.com/espressif/qemu.git && \
    cd qemu && \
    mkdir -p build && \
    cd build && \
    ../configure --target-list=xtensa-softmmu --without-default-features --enable-slirp --enable-gcrypt && \
    make -j $(nproc) vga=no && \
    make install && \
    rm -rf /opt/qemu

# Install PlatformIO platforms
RUN platformio platform install espressif32

# Create and set working directory
WORKDIR /app

CMD ["/bin/bash"]