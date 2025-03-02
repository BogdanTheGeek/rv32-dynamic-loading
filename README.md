# RISC-V Dynamic Loading
> [!WARNING]
> This project is a proof of concept and is not intended for production use.
This is an exploration of a system that allows for dynamic loading of "cross-platform" applications on any risc-v system. This is intended to emulate the behavior of a traditional operating system, but with a much simpler design.

## Introduction
For a long time, I have been thinking about writing a framework for creating portable apps for micro controllers.
Most projects resort to integrating a "light weight" interpreter like MicroPython, lua or berry. This has never sat right with me due to the horrible efficiency.
Not just in execution speed, but also FLASH/RAM usage.
I wanted a system closer to a real operating system and I think I got pretty close with this project.


## How does it work?
The "app" is compiled as position independent code.
All of the app's symbols are accessed though a GOT (Global Offset Table).
At load time, the [loader](kernel/loader) will load the app into memory and resolve the GOT.
After compilation, the app is packaged into a simple binary format with a header that can be used by the loader to resolve the GOT.

The entry point is always at the start of `.text` section.

All "kernel" functions are passed though a `sys` struct pointer which implements a simple jump table. This is always placed as the first item in `.data` section of the app.

The example kernel shows how FreeRTOS can be used to easily run the multiple apps concurrently.


## Aknowledgements
 - [riscv-elf-docs](https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-elf.adoc)
 - [rgujju's dynamic app loader for ARM](https://github.com/rgujju/Dynamic_App_Loading/tree/master)
 - [udynlink](https://github.com/bogdanm/udynlink)
