
TARGET = app
INCLUDES = -I../common 
LDSCRIPT = flat.ld
TOOLCHAIN = riscv64-unknown-elf
ELF2BTBF ?= ../tools/elf2btbf.py

PIC_FLAGS = -fPIE -fno-plt -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -mstrict-align
# PIC_FLAGS += -fPIC
CFLAGS = -march=rv32ic -mabi=ilp32 -Os -ggdb -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer $(PIC_FLAGS) -fno-semantic-interposition 
LDFLAGS = -Wl,--print-memory-usage -Wl,-Map=$(TARGET).map -L../common -T $(LDSCRIPT)  -nostdlib -Wl,--emit-relocs -Wl,--no-dynamic-linker -Wl,--relax -static-pie  -Wl,-Bstatic
# Comment out the next line if you want to keep unused global symbols
LDFLAGS += -Wl,--gc-sections

all: $(TARGET).btbf

$(TARGET).elf: main.c $(LDSCRIPT) Makefile
	$(TOOLCHAIN)-gcc -o $@ $< $(CFLAGS) $(INCLUDES) $(LDFLAGS)

$(TARGET).btbf: $(TARGET).elf
	$(ELF2BTBF) $<

dump: $(TARGET).elf
	$(TOOLCHAIN)-objdump --disassembler-color=on -D -S -j .text -j .data -j .bss -j .got $<

reloc: $(TARGET).elf
	$(TOOLCHAIN)-readelf -r $< | sed -n '/.rela.text/,/.rela.debug_info/p'

clean:
	rm -f $(TARGET).elf $(TARGET).map $(TARGET).btbf

