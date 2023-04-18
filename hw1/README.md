# Secured API Call

Implement secured api calls by library injection, API hijacking, and GOT rewriting

## Step

1. Hijack `__libc_start_main`
   Define a function whose name is `__libc_start_main` in `sandbox.c` so that it can override the original one using `LD_PRELOAD`. In our `__libc_start_main`, we can initialize anything you want.

2. Hijack api calls
   In `__libc_start_main`, I get the got offset using `readelf -r` and process starting memory address by `/proc/self/maps`. With the information, we can hijack the got entries with our own custom function.

3. Implement custom functions
   In the spec, we need to implement `open`, `read`, `write`, `connect`, `getaddrinfo` and `system`. The other details refer to the spec.

## Gain

#### PIE

> PIE binaries are compiled with position-independent code (PIC), which allows the code to be executed at any memory address without requiring any modifications to the code itself. The binary also includes a relocation table, which contains information about how to modify the code and data to run correctly at the chosen memory address.

-> so that the same library code can be loaded at a location in each program's address space where it does not overlap with other memory in use by

#### ALSR

> Address space layout randomization (ASLR) is a computer security technique involved in preventing exploitation of memory corruption vulnerabilities. In order to prevent an attacker from reliably jumping to, for example, a particular exploited function in memory, ASLR randomly arranges the address space positions of key data areas of a process, including the base of the executable and the positions of the stack, heap and libraries.

-> But it's not a really random, the last 12 bits are fixed

#### RELRO (RELocation Read Only)

There're three types of safety

- `No RELRO` - Link Map and GOT are writable
- `Partial RELRO` - GOT is writable
- `Full RELRO` - Both are not writable

Due to this, we need to use `mprotect` to make the page writable before we overwrite the got entries.

#### open

You can use the following macro to determine if `open` contains mode parameter

```c
#ifdef __O_TMPFILE
#define __OPEN_NEEDS_MODE(oflag) \
  (((oflag) & O_CREAT) != 0 || ((oflag) & __O_TMPFILE) == __O_TMPFILE)
#else
#define __OPEN_NEEDS_MODE(oflag) (((oflag) & O_CREAT) != 0)
#endif
```
