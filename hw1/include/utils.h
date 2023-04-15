#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool is_start_with(const char *str, const char *prefix);
uintptr_t get_base_addr();
ptrdiff_t get_offset(const char *symbol);
void make_writable(uintptr_t addr);