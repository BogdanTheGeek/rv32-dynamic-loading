#!/usr/bin/env python
import sys
import logging
import struct

from elftools.elf.elffile import ELFFile
from elftools import elf

elf_path = sys.argv[1]
app_name = elf_path.split('.')[0]

logging.basicConfig(level=logging.DEBUG)

def createBTBF(app_name, text, data, bss_size, got, major_version=1, minor_version=0):

    if len(app_name) > 15:
        app_name = app_name[:15]
        logging.warning(f"App name is greater than 15 characters, truncating to '{app_name}'")
    else:
        padding = 16 - len(app_name)
        app_name += padding * '\0'

    logging.info(f"App name: {app_name}")
    logging.info(f"Major version: {major_version}")
    logging.info(f"Minor version: {minor_version}")
    logging.info(f"Text size: {len(text)/4}")
    logging.info(f"Data size: {len(data)/4}")
    logging.info(f"BSS size: {bss_size}")
    logging.info(f"GOT size: {len(got)/4}")

    tinf = bytearray("BTBF",encoding='utf8')
    tinf += app_name.encode('utf8')
    tinf += struct.pack('<B',major_version)
    tinf += struct.pack('<B',minor_version)
    tinf += struct.pack('<H',int(len(text)/4))
    tinf += struct.pack('<H',int(len(data)/4))
    tinf += struct.pack('<H',int(bss_size))
    tinf += struct.pack('<H',int(len(got)/4))
    tinf += text
    tinf += data
    tinf += got

    return tinf



# Open the input elf file
with open(elf_path, 'rb') as f:
    # Read elf file
    elffile = ELFFile(f)

    # layout:
    # - .text section
    # - .data section
    # - .got section
    # - .bss section

    # get .text section
    text = elffile.get_section_by_name('.text')
    if not isinstance(text, elf.sections.Section):
        logging.error("ELF file has no .text section")
        sys.exit(-1)

    # get .data section
    data = elffile.get_section_by_name('.data')
    if not data:
        logging.error("ELF file has no .data section")
        sys.exit(-1)

    # get .got section
    got = elffile.get_section_by_name('.got')
    if not got:
        logging.error("ELF file has no .got section")
        sys.exit(-1)

    # get .bss section
    bss = elffile.get_section_by_name('.bss')
    if not bss:
        logging.error("ELF file has no .bss section")
        sys.exit(-1)

    rela_data = elffile.get_section_by_name('.rela.data')
    if rela_data:
        logging.warning("App contains .rela.data section. Loader doesn't not support relocation of global pointers, yet. Code may not work as expected.")

    bin_data = createBTBF(app_name, text.data(), data.data(), bss['sh_size'], got.data())

    with open(app_name + '.btbf', 'wb') as f:
        f.write(bin_data)
        logging.info(f"Written {app_name}.btbf")

