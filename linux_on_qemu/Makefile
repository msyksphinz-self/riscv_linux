build:
	docker build -t qemu-riscv-runner .
	$(MAKE) build-qemu

run:
	docker run --rm -it -v "${HOME}:${HOME}" --user $(shell id -u):$(shell id -g) -w "${PWD}" qemu-riscv-runner

QEMU_VERSION=10.1.0
build-qemu: .build_qemu
.build_qemu:
	wget https://download.qemu.org/qemu-$(QEMU_VERSION).tar.xz
	tar xJf qemu-$(QEMU_VERSION).tar.xz
	cd qemu-$(QEMU_VERSION) && \
		mkdir -p build && cd build && \
	    ../configure --target-list=riscv64-softmmu,riscv64-linux-user && \
		make -j32
	touch $@

create_qemu_command:
	echo ./qemu-$(QEMU_VERSION)/build/qemu-system-riscv64 -nographic -machine virt \
     -kernel /linux_riscv/linux/arch/riscv/boot/Image -append \"root=/dev/vda ro console=ttyS0\"  -drive file=/linux_riscv/busybox/busybox,format=raw,id=hd0 -device virtio-blk-device,drive=hd0 > qemu_riscv_boot.sh

clean:
	rm -rf .build_qemu qemu-$(QEMU_VERSION).tar.xz
