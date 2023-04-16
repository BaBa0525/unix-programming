#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

extern int LOGGER_FD;
#define Logger(fmt, ...)                                                  \
    do {                                                                  \
        char log_info[1024] = {};                                         \
        int write_bytes =                                                 \
            snprintf(log_info, PATH_MAX, "[logger] " fmt, ##__VA_ARGS__); \
        write(LOGGER_FD, log_info, write_bytes);                          \
    } while (0)

bool is_start_with(const char *str, const char *prefix);
uintptr_t get_base_addr();
ptrdiff_t get_offset(const char *symbol);
void make_writable(uintptr_t addr);
void get_log_path(int fd, char *fn_type, char *log_path);