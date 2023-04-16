#include "utils.h"

#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

bool is_start_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

uintptr_t get_base_addr() {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    uintptr_t addr;
    fscanf(fp, "%lx", &addr);
    fclose(fp);

    return addr;
}

void make_writable(uintptr_t addr) {
    size_t pagesize = getpagesize();

    // Round down to the nearest page boundary
    addr = addr & ~(pagesize - 1);
    if (mprotect((void *)addr, pagesize, PROT_READ | PROT_WRITE) == -1) {
        fprintf(stderr, "mprotect() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

ptrdiff_t get_offset(const char *symbol) {
    char *LD_PRELOAD = getenv("LD_PRELOAD");
    unsetenv("LD_PRELOAD");

    char exe_path[PATH_MAX] = {};
    readlink("/proc/self/exe", exe_path, sizeof(exe_path));

    char syscmd[PATH_MAX * 2] = {};
    snprintf(syscmd, PATH_MAX * 2, "readelf -r %s | grep ' %s@'", exe_path,
             symbol);
    FILE *pipe = popen(syscmd, "r");
    ptrdiff_t offset = 0;
    fscanf(pipe, "%lx", &offset);
    pclose(pipe);

    setenv("LD_PRELOAD", LD_PRELOAD, 1);

    return offset;
}

void get_log_path(int fd, char *fn_type, char *log_path) {
    pid_t pid = getpid();
    snprintf(log_path, PATH_MAX, "./logs/%d-%d-%s", pid, fd, fn_type);
}

bool cmp_ip(struct in_addr addr, char *hostname) {
    struct addrinfo hints = {}, *result, *rp;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(hostname, NULL, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return false;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)rp->ai_addr;
        if (addr_in->sin_addr.s_addr == addr.s_addr) return true;
    }
    return false;
}