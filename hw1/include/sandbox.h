#pragma once

#define CONFIG_PATH "/home/baba/lab/hw1/config.txt"

#define OPEN "open"
#define READ "read"
#define WRITE "write"
#define CONNECT "connect"
#define GETADDRINFO "getaddrinfo"
#define SYSTEM "system"

typedef int (*__libc_start_main_t)(int (*main)(int, char **, char **), int argc,
                                   char **ubp_av, void (*init)(void),
                                   void (*fini)(void), void (*rtld_fini)(void),
                                   void(*stack_end));

int __libc_start_main(int (*main)(int, char **, char **), int argc,
                      char **ubp_av, void (*init)(void), void (*fini)(void),
                      void (*rtld_fini)(void), void(*stack_end));
void hijack_api_calls();
void read_config(char *buf, const char fn_type[]);
