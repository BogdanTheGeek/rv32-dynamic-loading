# PIC vs PIE
PIE uses PC relative addressing, so no GOT is needed.

PIC uses a GOT to access global data which means that .data is the only section that needs to be in RAM.

Both require relocations to be applied at load time.
Both can be used as shared libraries if I manage the symbols during packing.

# Relocations
Section relocations are stored in .rela.data.
The loader will need to add the address of the entry point to the contents of the symbol that needs to be relocated.

```c
int x = 0;
int *p = &x;
```
Compiles to:
```
    .data
0030: .word 0    // x = 0
0034: .word 0x30 // p = &x
```
In the above, the second word is a pointer to the first word.
But this is incorrect because the app could be loaded anywhere in RAM.

Section .rela.data will contain:
```
Relocation section '.rela.data' at offset 0x22ac contains 1 entry:
 Offset     Info    Type            Sym.Value  Sym. Name + Addend
00000034  00007001 R_RISCV_32        00000030   x + 0
```
The loader will need to add the address of the entry point to the value at offset 0x34.

