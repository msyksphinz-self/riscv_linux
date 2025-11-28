CROSS_COMPILE ?= riscv64-unknown-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CFLAGS = -static -O2

TESTS = hello test_syscall

all: $(TESTS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TESTS)

.PHONY: all clean

