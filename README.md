# RISC-V Dynamic Loading
Example implementation of a risc-v dynamic loader for embedded application.

## Introduction
> [!WARNING]
> This project is a proof of concept and is not intended for production use.
This is an exploration of a system that allows for dynamic loading of "cross-platform" applications on any risc-v system. This is intended to emulate the behavior of a traditional operating system, but with a much simpler design.


## How does it work?
The "app" is compiled as position independent code.
All of the app's symbols are accessed though a GOT (Global Offset Table).
At load time, the [loader](kernel/loader) will load the app into memory and resolve the GOT.
After compilation, the app is packaged into a simple binary format with a header that can be used by the loader to resolve the GOT.

The entry point is always at the start of `.text` section.

All "kernel" functions are passed though a `sys` struct pointer which implements a simple jump table. This is always placed as the first item in `.data` section of the app.

The example kernel shows how freeRTOS can be used to easily run the multiple apps concurrently.


## Aknowledgements
 - [riscv-elf-docs](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-elf.adoc)
 - [rgujju's dynamic app loader for ARM](https://github.com/rgujju/Dynamic_App_Loading/tree/master)
 - [udynlink](https://github.com/bogdanm/udynlink)
