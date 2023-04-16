#pragma once

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>

#ifndef __OPEN_NEEDS_MODE
/** https://elixir.bootlin.com/glibc/latest/source/io/fcntl.h#L40 */
#ifdef __O_TMPFILE
#define __OPEN_NEEDS_MODE(oflag) \
    (((oflag)&O_CREAT) != 0 || ((oflag)&__O_TMPFILE) == __O_TMPFILE)
#else
#define __OPEN_NEEDS_MODE(oflag) (((oflag)&O_CREAT) != 0)
#endif
#endif

int secure_open(const char *pathname, int flags, ...);

ssize_t secure_read(int fd, void *buf, size_t count);

ssize_t secure_write(int fd, const void *buf, size_t count);

int secure_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int secure_getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints, struct addrinfo **res);

int secure_system(const char *command);