# RISC-V Linux on QEMU

A project to build and run RISC-V Linux on QEMU, including implementation and testing of custom PCI devices.

## Overview

This project includes the following components:

- **RISC-V GNU Toolchain**: Cross-compiler for RISC-V architecture
- **BusyBox**: Lightweight root filesystem
- **Linux Kernel (v6.9)**: RISC-V Linux kernel
- **OpenSBI (v1.6)**: RISC-V firmware
- **QEMU (v10.1.0)**: Emulator (includes custom PCI device implementation)

## Custom PCI Device

This project implements a custom PCI device (`my-accel`) in QEMU.

- **Vendor ID**: `0x1234`
- **Device ID**: `0x5678`
- **Class**: `PCI_CLASS_OTHERS` (0x00ff00)
- **BAR0**: 4KB MMIO region

For implementation details and troubleshooting, see `../qemu_pci_device_implementation_blog.md`.

## Requirements

- Docker
- Make
- Sufficient disk space (recommended: 10GB or more)

## Setup

### 1. Clone the Repository

```bash
git clone <repository-url>
cd linux_on_qemu
```

### 2. Build Docker Image

Build the build environment using Dockerfile:

```bash
make build
```

This command will:
- Build the Docker image (`qemu-riscv-runner`)
- Download and build QEMU v10.1.0

### 3. Build QEMU (Local)

While QEMU is built inside the Dockerfile, to build QEMU locally:

```bash
make build-qemu
```

This will generate `qemu-10.1.0/build/qemu-system-riscv64`.

## Running

### Run Inside Docker Container

```bash
make run
```

Then run the following command inside the container:

```bash
./qemu-10.1.0/build/qemu-system-riscv64 -nographic -machine virt \
  -kernel /linux_riscv/linux/arch/riscv/boot/Image \
  -initrd /linux_riscv/rootfs.cpio \
  -append "console=ttyS0 rdinit=/init"
```

### Run Locally

If using files built by the Dockerfile, you need to copy the necessary files from the container.

## Directory Structure

```
linux_on_qemu/
├── Dockerfile              # Build environment definition
├── Makefile               # Build and run management
├── qemu_riscv_boot.sh     # QEMU boot script
├── qemu-10.1.0/           # QEMU source code and build artifacts
│   └── hw/misc/my_accel.c # Custom PCI device implementation
├── tests/                 # Test programs
│   ├── hello.c
│   ├── test_syscall.c
│   └── Makefile
├── blog_article.md        # Blog article (RISC-V Linux environment setup)
├── rootfs.cpio            # BusyBox-based root filesystem
└── README.md              # This file
```

## Verifying Custom PCI Device

After QEMU starts, you can verify the custom PCI device in the guest OS with:

```bash
lspci -nn
```

Example output:

```
00:00.0 Class 0600: 1b36:0008
00:05.0 Class 00ff: 1234:5678
```

`00:05.0` is the custom PCI device (`my-accel`).

You can also check the kernel log:

```
[    0.336054] pci 0000:00:05.0: [1234:5678] type 00 class 0x00ff00 conventional PCI endpoint
[    0.338845] pci 0000:00:05.0: BAR 0 [mem 0x40000000-0x40000fff]: assigned
```

## Test Programs

The `tests/` directory contains test programs to run on QEMU.

### Mount Test Filesystem

After QEMU starts, mount the test filesystem with:

```bash
mount -t ext2 /dev/vdb /mnt/testfs
```

Test programs are located in `/mnt/testfs/bin/`.

### Adding Test Programs

1. Add a new C program to the `tests/` directory
2. Add the program name to the `TESTS` variable in `tests/Makefile`
3. Rebuild the Docker image, and it will be automatically compiled and included in `testfs.img`

## Troubleshooting

### QEMU Build Errors

If QEMU build fails, check that all required dependency packages are installed. Refer to the Dockerfile to see which packages need to be installed.

### Custom PCI Device Not Detected

If the custom PCI device is not detected, check:

1. Whether QEMU is built correctly
2. Whether `qemu-10.1.0/hw/misc/my_accel.c` exists
3. Whether `my_accel.c` is added to `qemu-10.1.0/hw/misc/meson.build`

For details, see `../qemu_pci_device_implementation_blog.md`.

## Cleanup

To remove build artifacts:

```bash
make clean
```

This will remove:
- `.build_qemu`
- `qemu-10.1.0.tar.xz`

## References

- [QEMU Official Documentation](https://www.qemu.org/documentation/)
- [RISC-V Linux Official Site](https://riscv.org/)
- [Custom PCI Device Implementation Troubleshooting](../qemu_pci_device_implementation_blog.md)

## License

Follow the licenses of each component used in this project.
