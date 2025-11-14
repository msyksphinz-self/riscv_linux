FROM ubuntu:24.04

# 必要なパッケージをインストール
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

# Build Spike
RUN git clone https://github.com/riscv-software-src/riscv-isa-sim.git && \
    cd riscv-isa-sim && \
    mkdir -p build && cd build && \
    ../configure --prefix=$RISCV && \
    make -j$(nproc) && make install && \
    cd ../ && rm -rf build

# Build QEMU (v9.2.4)
RUN git clone https://github.com/qemu/qemu.git --depth=1 -b v9.2.4 && \
    cd qemu && \
    mkdir -p build && cd build && \
    ../configure --target-list=riscv64-softmmu --prefix=$RISCV && \
    make -j$(nproc) && make install && \
    cd ../ && rm -rf build

RUN apt-get update && apt-get install -y \
    libncurses-dev

# Download BusyBox and create rootfs.cpio (v1.36.0)
RUN git clone https://git.busybox.net/busybox --depth=1 -b 1_36_0 && \
	cd busybox && \
	CROSS_COMPILE=riscv64-unknown-linux-gnu- make defconfig && \
    sed -i 's|# CONFIG_STATIC is not set|CONFIG_STATIC=y|g' .config && \
	CROSS_COMPILE=riscv64-unknown-linux-gnu- make -j$(nproc) && \
    CROSS_COMPILE=riscv64-unknown-linux-gnu- make install -j$(nproc) CONFIG_PREFIX=/linux_riscv/rootfs

# RUN mkdir -p /linux_riscv/rootfs && cd /linux_riscv/rootfs && \
#     echo "#!/bin/sh\n\
# mount -t proc proc /proc\n\
# mount -t sysfs sys /sys\n\
# mount -t devtmpfs dev /dev\n\
# echo Hello World0\n\
# mknod /dev/console c 5 1\n\
# echo Hello World1\n\
# chmod 600 /dev/console\n\
# exec /bin/sh"  > /linux_riscv/rootfs/init && chmod +x /linux_riscv/rootfs/init

RUN mkdir -p /linux_riscv/rootfs && cd /linux_riscv/rootfs && \
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
echo "== init: dev contents ==" >/dev/console 2>&1\n\
ls -l /dev                 >/dev/console 2>&1\n\
echo "==========================" >/dev/console 2>&1\n\
exec /bin/sh < /dev/console > /dev/console 2>&1"  > /linux_riscv/rootfs/init && chmod +x /linux_riscv/rootfs/init

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

# Make disk image from Busybox
# RUN dd if=/dev/zero of=root.bin bs=1M count=64 && \
# 	mkfs.ext2 -F root.bin && \
# 	mkdir -p mnt && \
# 	mount -o loop root.bin mnt && \
# 	cp -r busybox/_install/* mnt/ && \
# 	cd mnt && \
# 	sudo mkdir dev etc proc run sys && \
# 	cd dev && \
# 	sudo cp -a /dev/console . && \
# 	sudo cp -a /dev/tty[0-4] . && \
# 	cd ../.. && \
# 	umount mnt

# RUN mkdir -p /rootfs/bin /rootfs/dev /rootfs/proc /rootfs/sys && \
#     wget -O /rootfs/bin/sh https://busybox.net/downloads/binaries/1.35.0-defconfig-multiarch/busybox-riscv64 && \
#     chmod +x /rootfs/bin/sh && \
#     echo -e "#!/bin/sh\nexec /bin/sh" > /rootfs/init && \
#     chmod +x /rootfs/init && \
#     sudo mknod /rootfs/dev/console c 5 1

# RUN mkdir -p rootfs/bin rootfs/sbin rootfs/etc rootfs/proc rootfs/sys rootfs/usr/bin rootfs/usr/sbin rootfs/dev} && \
#     echo '#!/bin/sh' > rootfs/init && \
#     echo 'exec /bin/sh' >> rootfs/init && \
#     chmod +x rootfs/init && \
#     cd rootfs && \
#     find . | cpio -o --format=newc > ../rootfs.cpio

# 作業ディレクトリを設定
# WORKDIR /

# # 最後にエントリポイント（Spike起動）
# CMD spike \
#     --kernel /linux/arch/riscv/boot/Image \
#     --initrd /rootfs.cpio \
#     ./opensbi/build/platform/generic/firmware/fw_jump.elf \
#     root=/dev/ram rw init=/bin/init

CMD ["bash"]
