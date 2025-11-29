qemu-system-riscv64 -nographic -machine virt \
                    -kernel /linux_riscv/linux/arch/riscv/boot/Image \
                    -initrd /linux_riscv/rootfs.cpio \
                    -drive file=/linux_riscv/testfs.img,format=raw,id=testfs,if=none \
                    -device virtio-blk-device,drive=testfs \
                    -append "console=ttyS0 rdinit=/init"
