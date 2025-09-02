#!/usr/bin/env python
import sys
import logging
import struct

from elftools.elf.elffile import ELFFile
from elftools import elf

elf_path = sys.argv[1]
app_name = elf_path.split(".")[0]

logging.basicConfig(level=logging.DEBUG)


def siz(arr):
    return (len(arr) + 3) // 4


def createBTBF(app_name, text, data, bss, got, syms, major_version=1, minor_version=0):
    if len(app_name) > 15:
        app_name = app_name[:15]
        logging.warning(
            f"App name is greater than 15 characters, truncating to '{app_name}'"
        )
    else:
        padding = 16 - len(app_name)
        app_name += padding * "\0"

    logging.info(f"App name: {app_name}")
    logging.info(f"Major version: {major_version}")
    logging.info(f"Minor version: {minor_version}")
    logging.info(f"Text size: {siz(text)}")
    logging.info(f"Data size: {siz(data)}")
    logging.info(f"BSS size: {siz(bss)}")
    logging.info(f"GOT size: {siz(got)}")

    tinf = bytearray("BTBF", encoding="utf8")
    tinf += app_name.encode("utf8")
    tinf += struct.pack("<B", major_version)
    tinf += struct.pack("<B", minor_version)
    tinf += struct.pack("<H", siz(text))
    tinf += struct.pack("<H", siz(data))
    tinf += struct.pack("<H", siz(bss))
    tinf += struct.pack("<H", siz(got))
    tinf += text
    tinf += data
    tinf += bss
    tinf += got

    # TODO: Add symbol table and relocations

    return tinf


# Open the input elf file
with open(elf_path, "rb") as f:
    # Read elf file
    elffile = ELFFile(f)

    # layout:
    # - .text section
    # - .data section
    # - .got section
    # - .bss section

    # get .text section
    text = elffile.get_section_by_name(".text")
    if not isinstance(text, elf.sections.Section):
        logging.error("ELF file has no .text section")
        sys.exit(-1)

    # get .data section
    data = elffile.get_section_by_name(".data")
    if not data:
        logging.error("ELF file has no .data section")
        sys.exit(-1)

    # get .got section
    got = elffile.get_section_by_name(".got")
    got_data = bytearray()
    if not got:
        logging.warning("ELF file has no .got section")
    else:
        got_data = bytearray(got.data())[:-8]  # remove .got.plt

    # get .bss section
    bss = elffile.get_section_by_name(".bss")
    bss_data = bytearray()
    if not bss:
        logging.warning("ELF file has no .bss section")
    else:
        bss_data = bytearray(bss.data())

    symtab = elffile.get_section_by_name(".symtab")
    if not symtab:
        logging.error("ELF file has no symbol table")
        sys.exit(-1)

    syms = {}
    for sym in symtab.iter_symbols():
        is_func = sym["st_info"]["type"] == "STT_FUNC"
        is_glob = sym["st_info"]["bind"] == "STB_GLOBAL"
        if is_glob and sym.name != "main":
            type_str = "Function" if is_func else "Variable"
            logging.debug(f"Global {type_str}: {sym.name}@{hex(sym['st_value'])}")
            syms[sym["st_value"]] = {
                "name": sym.name,
                "type": "func" if is_func else "var",
                "reloc": False,
            }

    rela_data = elffile.get_section_by_name(".rela.data")
    if rela_data:
        logging.warning(
            "App contains .rela.data section. Loader doesn't not support relocation of global pointers, yet. Code may not work as expected."
        )
        for rel in rela_data.iter_relocations():
            addr = rel["r_offset"]
            name = syms[addr]["name"]
            syms[addr]["reloc"] = True
            logging.debug(f"Symbol {name}@{hex(addr)} needs relocation")

    bin_data = createBTBF(
        app_name=app_name,
        text=text.data(),
        data=data.data(),
        bss=bss_data,
        got=got_data,
        syms=syms,
    )

    with open(app_name + ".btbf", "wb") as f:
        f.write(bin_data)
        logging.info(f"Written {app_name}.btbf")
        logging.info(f"Size: {len(bin_data)} bytes")
