#include "sandbox.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "override.h"
#include "utils.h"

int __libc_start_main(int (*main)(int, char **, char **), int argc,
                      char **ubp_av, void (*init)(void), void (*fini)(void),
                      void (*rtld_fini)(void), void(*stack_end)) {
    // This is the function that is called when the program starts.
    // We can use this to do some initialization before the program starts.

    void *handle = dlopen(LIBC_PATH, RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, ubp_av[i]);
    }

    hijack_api_calls();

    printf("The value of the environment variable LOGGER_FD is: %s\n",
           getenv("LOGGER_FD"));
    __libc_start_main_t start_fn = dlsym(handle, "__libc_start_main");
    return start_fn(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

void hijack_api_calls() {
    uintptr_t base_addr = get_base_addr();
    void *fn_ptr[] = {secure_open,    secure_read,        secure_write,
                      secure_connect, secure_getaddrinfo, secure_system};
    const char api_calls[][256] = {OPEN,    READ,        WRITE,
                                   CONNECT, GETADDRINFO, SYSTEM};

    for (int i = 0; i < sizeof(api_calls) / sizeof(api_calls[0]); i++) {
        printf("hijacking %s...\n", api_calls[i]);
        uintptr_t got_offset = get_offset(api_calls[i]);
        uintptr_t got_addr = base_addr + got_offset;
        make_writable(got_addr);
        memcpy((void *)got_addr, &fn_ptr[i], sizeof(void *));
    }
}

void read_config(char *buf, const char fn_type[]) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (fp == NULL) {
        fprintf(stderr, "fopen() error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char start[256] = {}, end[256] = {}, line[256] = {};
    char *line_ptr = line;
    snprintf(start, 256, "BEGIN %s-blacklist", fn_type);
    snprintf(end, 256, "END %s-blacklist", fn_type);

    bool is_start = false;
    size_t nbytes = 0, len = 256;
    while ((nbytes = getline(&line_ptr, &len, fp)) != -1) {
        if (is_start_with(line_ptr, start)) {
            is_start = true;
            continue;
        }
        if (!is_start) continue;
        if (is_start_with(line_ptr, end)) break;

        strncpy(buf, line_ptr, nbytes);
        buf += nbytes;
    }
}