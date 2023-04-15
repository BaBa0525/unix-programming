#include "sandbox.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

    call_readelf();

    printf("The value of the environment variable LOGGER_FD is: %s\n",
           getenv("LOGGER_FD"));
    __libc_start_main_t start_fn = dlsym(handle, "__libc_start_main");
    return start_fn(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}

void call_readelf() {
    unsetenv("LD_PRELOAD");

    char exe_path[100] = {};
    readlink("/proc/self/exe", exe_path, sizeof(exe_path));

    char syscmd[100] = {};
    snprintf(syscmd, 120, "readelf -r %s", exe_path);
    system(syscmd);
}