#include "override.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sandbox.h"
#include "utils.h"

int secure_open(const char *pathname, int flags, mode_t mode) {
    printf("secure_open() is called with pathname: %s, flags: %d, mode: %d\n",
           pathname, flags, mode);
    char blacklist[PATH_MAX] = {}, real_pathname[PATH_MAX];
    if (realpath(pathname, real_pathname) == NULL) {
        printf("realpath() error: %s\n", strerror(errno));
        return -1;
    }
    read_config(blacklist, "open");

    char *blocked_path = strtok(blacklist, "\n");

    while (blocked_path != NULL) {
        char blocked_real_path[PATH_MAX] = {};
        if (realpath(blocked_path, blocked_real_path) == NULL) {
            printf("realpath() error: %s\n", strerror(errno));
            return -1;
        }
        if (strcmp(real_pathname, blocked_real_path) == 0) {
            printf("open() is blocked for %s\n", real_pathname);
            errno = EACCES;
            return -1;
        }
        blocked_path = strtok(NULL, "\n");
    }

    return open(real_pathname, flags, mode);
}

int secure_read(int fd, void *buf, size_t count) {
    printf("secure_read() is called with fd: %d, buf: %p, count: %zu\n", fd,
           buf, count);
    return read(fd, buf, count);
}

int secure_write(int fd, const void *buf, size_t count) {
    printf("secure_write() is called with fd: %d, buf: %p, count: %zu\n", fd,
           buf, count);
    return write(fd, buf, count);
}

int secure_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf(
        "secure_connect() is called with sockfd: %d, addr: %p, addrlen: %d\n",
        sockfd, addr, addrlen);
    return connect(sockfd, addr, addrlen);
}

int secure_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints, struct addrinfo **res) {
    printf(
        "secure_getaddrinfo() is called with node: %s, service: %s, hints: %p, "
        "res: %p\n",
        node, service, hints, res);
    return getaddrinfo(node, service, hints, res);
}

int secure_system(const char *command) {
    printf("secure_system() is called with command: %s\n", command);
    return system(command);
}