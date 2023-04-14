#include <stdio.h>

typedef int (*printf_ptr_t)(const char *format, ...);

void solver(printf_ptr_t fptr) {
    char msg[8] = {};
    fptr("%llu\n", *(unsigned long long *)&msg[8]);
    fptr("%llu\n", *(unsigned long long *)&msg[16]);
    fptr("%llu\n", *(unsigned long long *)&msg[24]);
}

int main() {
    char fmt[16] = "** main = %p\n";
    printf(fmt, main);
    solver(printf);
    return 0;
}