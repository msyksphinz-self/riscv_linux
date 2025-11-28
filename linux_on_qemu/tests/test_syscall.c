#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>

int main() {
    struct utsname info;
    if (uname(&info) == 0) {
        printf("System: %s\n", info.sysname);
        printf("Node: %s\n", info.nodename);
        printf("Release: %s\n", info.release);
        printf("Version: %s\n", info.version);
        printf("Machine: %s\n", info.machine);
    }
    printf("Current working directory: ");
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    }
    return 0;
}

