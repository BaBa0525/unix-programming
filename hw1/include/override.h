#pragma once

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>

int secure_open(const char *pathname, int flags, mode_t mode);

ssize_t secure_read(int fd, void *buf, size_t count);

ssize_t secure_write(int fd, const void *buf, size_t count);

int secure_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int secure_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints, struct addrinfo **res);

int secure_system(const char *command);