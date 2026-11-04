#!/usr/bin/env python3
"""Derive the test/readelf/*.elf variants from test/readelf/rv32i.elf.

rv32i.elf is a riscv-tests style rv32i+zicsr image, everything else is a patched
copy of it. Run from anywhere: python3 test/readelf/make_fixtures.py
"""

import struct
from pathlib import Path

TEST_DIR = Path(__file__).resolve().parent
BASE = TEST_DIR / "rv32i.elf"

EI_DATA = 5
EI_DATA_BE = 2

# Elf32 field layouts after e_ident, without the byte-order prefix
EHDR = "HHIIIIIHHHHHH"  # e_type .. e_shstrndx
PHDR = "IIIIIIII"  # p_type .. p_align
SHDR = "IIIIIIIIII"  # sh_name .. sh_entsize


def unpack_ehdr(elf: bytes) -> list:
    return list(struct.unpack_from("<" + EHDR, elf, 16))


# e_* field positions in the unpacked EHDR tuple
E_ENTRY, E_PHOFF, E_SHOFF, E_FLAGS = 3, 4, 5, 6
E_PHNUM, E_SHNUM = 9, 11


def patch_ehdr(elf: bytes, field: int, value: int) -> bytes:
    out = bytearray(elf)
    fields = unpack_ehdr(elf)
    fields[field] = value
    struct.pack_into("<" + EHDR, out, 16, *fields)
    return bytes(out)


def to_big_endian(elf: bytes) -> bytes:
    """Byte-swap every header table. Section contents are left alone: RISC-V
    instructions are little-endian regardless of EI_DATA, and readelf -h/-l
    never look inside sections."""
    out = bytearray(elf)
    fields = unpack_ehdr(elf)
    out[EI_DATA] = EI_DATA_BE
    struct.pack_into(">" + EHDR, out, 16, *fields)

    def swap_table(offset: int, count: int, layout: str) -> None:
        size = struct.calcsize("<" + layout)
        for i in range(count):
            at = offset + i * size
            entry = struct.unpack_from("<" + layout, elf, at)
            struct.pack_into(">" + layout, out, at, *entry)

    swap_table(fields[E_PHOFF], fields[E_PHNUM], PHDR)
    swap_table(fields[E_SHOFF], fields[E_SHNUM], SHDR)
    return bytes(out)


EF_RISCV_RVE = 0x8
EF_RISCV_TSO = 0x10

VARIANTS = {
    "rv32i_rve.elf": lambda e: patch_ehdr(e, E_FLAGS, EF_RISCV_RVE),
    "rv32i_tso.elf": lambda e: patch_ehdr(e, E_FLAGS, EF_RISCV_TSO),
    "rv32i_rve_tso.elf": lambda e: patch_ehdr(
        e, E_FLAGS, EF_RISCV_RVE | EF_RISCV_TSO
    ),
    "rv32i_entry4.elf": lambda e: patch_ehdr(
        e, E_ENTRY, unpack_ehdr(e)[E_ENTRY] + 4
    ),
    "rv32i_be.elf": to_big_endian,
}


def main() -> None:
    base = BASE.read_bytes()
    for name, make in VARIANTS.items():
        (TEST_DIR / name).write_bytes(make(base))
        print(f"wrote {name}")


if __name__ == "__main__":
    main()
