FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    git \
    make \
    autoconf \
    automake \
    libtool \
    build-essential \
    device-tree-compiler \
    cpio \
    libssl-dev \
    wget \
    bc \
    flex \
    bison \
    python3 \
    curl \
    autotools-dev \
    python3-pip \
    python3-tomli \
    libmpc-dev \
    libmpfr-dev \
    libgmp-dev \
    gawk \
    texinfo \
    gperf  \
    patchutils \
    bc \
    zlib1g-dev \
    libexpat-dev \
    ninja-build \
    cmake \
    libglib2.0-dev \
    libslirp-dev \
    sudo \
    libpixman-1-dev \
    libnuma-dev \
    e2fsprogs \
    genext2fs \
    && apt-get clean

# Environment
ENV RISCV=/opt/riscv
ENV PATH=$RISCV/bin:$PATH
ENV LD_LIBRARY_PATH=$RISCV/lib

RUN mkdir -p $RISCV
WORKDIR /linux_riscv/

# Download riscv64-unknown-linux-gnu-gcc

RUN cd $RISCV/../ && \
    curl -L https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/2025.05.10/riscv64-glibc-ubuntu-24.04-gcc-nightly-2025.05.10-nightly.tar.xz | tar Jx

# Download BusyBox and create rootfs.cpio (v1.36.0)
RUN git clone https://git.busybox.net/busybox --depth=1 -b 1_36_0 && \
    cd busybox && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig && \
    sed -i 's|# CONFIG_STATIC is not set|CONFIG_STATIC=y|g' .config && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- make -j$(nproc) && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- make install -j$(nproc) CONFIG_PREFIX=/linux_riscv/rootfs

RUN apt-get update && apt-get install -y \
    libncurses-dev

RUN mkdir -p /linux_riscv/rootfs && cd /linux_riscv/rootfs && \
mkdir -p dev proc sys mnt &&\
    echo "#!/bin/sh\n\
echo "[init] start" > /dev/console 2>&1\n\
mount -t proc    proc  /proc\n\
mount -t sysfs   sys   /sys\n\
mount -t devtmpfs dev   /dev 2>/dev/null || mount -t tmpfs    tmp   /dev\n\
mknod /dev/console c 5 1     2>/dev/null || true\n\
mknod /dev/ttyS0   c 4 64    2>/dev/null || true\n\
mknod /dev/null    c 1 3     2>/dev/null || true\n\
chmod 600 /dev/console     2>/dev/null || true\n\
chmod 666 /dev/null        2>/dev/null || true\n\
mkdir -p /dev/pts\n\
mount -t devpts devpts /dev/pts 2>/dev/null || true\n\
# Mount test filesystem if available\n\
if [ -b /dev/vdb ]; then\n\
    mkdir -p /mnt/testfs\n\
    mount -t ext2 /dev/vdb /mnt/testfs 2>/dev/null && echo "[init] testfs mounted at /mnt/testfs" > /dev/console 2>&1 || echo "[init] failed to mount testfs" > /dev/console 2>&1\n\
fi\n\
exec setsid /bin/sh -i </dev/console >/dev/console 2>&1"  > /linux_riscv/rootfs/init && chmod +x /linux_riscv/rootfs/init

# RUN echo "#!/bin/sh\nmount -t proc none /proc\necho Hello World\n\nexec /bin/sh" > /linux_riscv/rootfs/init && chmod +x /linux_riscv/rootfs/init
RUN cd /linux_riscv/rootfs && \
    find . -print0 | cpio --null -ov --format=newc > /linux_riscv/rootfs.cpio

# Build Linux (v6.9)
RUN git clone --depth=1 https://github.com/torvalds/linux.git -b v6.9 && \
    cd linux && \
    make ARCH=riscv defconfig && \
    sed -i 's|CONFIG_INITRAMFS_SOURCE=""|CONFIG_INITRAMFS_SOURCE="/linux_riscv/rootfs.cpio"|g' .config && \
    make ARCH=riscv CROSS_COMPILE=riscv64-unknown-linux-gnu- -j$(nproc)

# Build OpenSBI (v1.6)
RUN git clone https://github.com/riscv-software-src/opensbi.git --depth 1 -b v1.6 && \
    cd opensbi && \
    make CROSS_COMPILE=riscv64-unknown-linux-gnu- \
    PLATFORM=generic FW_PAYLOAD_PATH=/linux_riscv/linux/arch/riscv/boot/Image

# Build test programs and create test filesystem image
RUN mkdir -p /linux_riscv/testfs && \
    mkdir -p /linux_riscv/testfs/bin
COPY tests/ /linux_riscv/tests/
RUN cd /linux_riscv/tests && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- make && \
    cp hello test_syscall /linux_riscv/testfs/bin/ && \
    cd /linux_riscv/tests && make clean

# Create ext2 filesystem image for tests
RUN genext2fs -b 16384 -d /linux_riscv/testfs /linux_riscv/testfs.img && \
    rm -rf /linux_riscv/testfs
    
# Build QEMU (v9.2.4)
RUN git clone https://github.com/qemu/qemu.git --depth=1 -b v9.2.4 && \
    cd qemu && \
    mkdir -p build && cd build && \
    ../configure --target-list=riscv64-softmmu --prefix=$RISCV && \
    make -j$(nproc) && make install && \
    cd ../ && rm -rf build
