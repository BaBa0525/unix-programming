// Environment variable: LOGGER_FD
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define LIBC_PATH "/usr/lib/x86_64-linux-gnu/libc.so.6"

typedef int (*__libc_start_main_t)(int (*main)(int, char **, char **), int argc,
                                   char **ubp_av, void (*init)(void),
                                   void (*fini)(void), void (*rtld_fini)(void),
                                   void(*stack_end));

int __libc_start_main(int (*main)(int, char **, char **), int argc,
                      char **ubp_av, void (*init)(void), void (*fini)(void),
                      void (*rtld_fini)(void), void(*stack_end)) {
    // This is the function that is called when the program starts.
    // We can use this to do some initialization before the program starts.
    // TODO: Call the original __libc_start_main() function.

    void *handle = dlopen(LIBC_PATH, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    __libc_start_main_t start_fn = dlsym(handle, "__libc_start_main");

    printf("Hello world!\n");
    return start_fn(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
}