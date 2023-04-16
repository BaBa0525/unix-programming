#include "override.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sandbox.h"
#include "utils.h"

int secure_open(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    if (__OPEN_NEEDS_MODE(flags)) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }

    char blacklist[PATH_MAX] = {}, real_pathname[PATH_MAX];
    if (realpath(pathname, real_pathname) == NULL) {
        perror("realpath()");
        Logger("open(\"%s\", %d, %hu) = -1\n", pathname, flags, mode);
        return -1;
    }
    read_config(blacklist, OPEN);

    char *blocked_path = strtok(blacklist, "\n");

    while (blocked_path != NULL) {
        char blocked_real_path[PATH_MAX] = {};
        if (realpath(blocked_path, blocked_real_path) == NULL) {
            perror("realpath()");
            Logger("open(\"%s\", %d, %hu) = -1\n", pathname, flags, mode);
            return -1;
        }
        if (strcmp(real_pathname, blocked_real_path) == 0) {
            errno = EACCES;
            Logger("open(\"%s\", %d, %x) = -1\n", pathname, flags, mode);
            return -1;
        }
        blocked_path = strtok(NULL, "\n");
    }

    int ret = open(real_pathname, flags, mode);
    Logger("open(\"%s\", %d, %hu) = %d\n", pathname, flags, mode, ret);
    return ret;
}

size_t match_cursor = 0;
ssize_t secure_read(int fd, void *buf, size_t count) {
    char read_buf[131072] = {};
    int nbytes = read(fd, read_buf, count);
    if (nbytes == -1) {
        Logger("read(%d, %12p, %lu) = %d\n", fd, buf, count, nbytes);
        return -1;
    }

    char blocked_str[1024] = {};
    read_config(blocked_str, READ);
    blocked_str[strlen(blocked_str) - 1] = '\0';

    for (int i = 0; i < nbytes; i++) {
        if (read_buf[i] != blocked_str[match_cursor]) {
            match_cursor = 0;
            continue;
        }
        match_cursor++;
        if (match_cursor == strlen(blocked_str)) {
            match_cursor = 0;
            close(fd);
            Logger("read(%d, %12p, %lu) = -1\n", fd, buf, count);
            errno = EIO;
            return -1;
        }
    }

    memcpy(buf, read_buf, nbytes);

    char log_file_name[PATH_MAX] = {};
    get_log_path(fd, READ, log_file_name);
    FILE *fp = fopen(log_file_name, "a");
    if (fp == NULL) {
        perror("fopen()");
        Logger("read(%d, %12p, %lu) = -1\n", fd, buf, count);
        return -1;
    }
    fwrite(buf, 1, nbytes, fp);
    fclose(fp);
    Logger("read(%d, %12p, %lu) = %d\n", fd, buf, count, nbytes);

    return nbytes;
}

ssize_t secure_write(int fd, const void *buf, size_t count) {
    Logger("write(%d, %12p, %zu) = %zu\n", fd, buf, count, count);
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