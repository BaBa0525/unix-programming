#include "override.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sandbox.h"
#include "utils.h"

int secure_open(const char *pathname, int flags, mode_t mode) {
    printf("secure_open() is called with pathname: %s, flags: %d, mode: %d\n",
           pathname, flags, mode);
    return open(pathname, flags, mode);
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