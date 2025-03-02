
TARGET = app
INCLUDES = -I../common 
LDSCRIPT = flat.ld
TOOLCHAIN = riscv64-elf
ELF2BTBF ?= ../tools/elf2btbf.py

PIC_FLAGS = -fPIE -static-pie -fPIC -fno-plt -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -mstrict-align
CFLAGS = -march=rv32ic -mabi=ilp32 -Os -fno-builtin -ffunction-sections -fdata-sections -fomit-frame-pointer $(PIC_FLAGS)
LDFLAGS = -Wl,--print-memory-usage -Wl,-Map=$(TARGET).map -L../common -T $(LDSCRIPT) -Wl,--gc-sections -nostdlib -Wl,--emit-relocs -Wl,--no-dynamic-linker

all: $(TARGET).btbf

$(TARGET).elf: main.c
	$(TOOLCHAIN)-gcc -o $@ $^ $(CFLAGS) $(INCLUDES) $(LDFLAGS)

$(TARGET).btbf: $(TARGET).elf
	$(ELF2BTBF) $<

dump: $(TARGET).elf
	$(TOOLCHAIN)-objdump -D $<

reloc: $(TARGET).elf
	$(TOOLCHAIN)-readelf -r $<

clean:
	rm -f $(TARGET).elf $(TARGET).map $(TARGET).btbf

