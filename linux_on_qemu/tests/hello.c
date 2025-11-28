#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello from RISC-V test program!\n");
    printf("PID: %d\n", getpid());
    return 0;
}

