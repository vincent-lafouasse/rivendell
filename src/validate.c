#include "validate.h"

#include <stdbool.h>
#include <string.h>

#define MAGIC "\x7f\x45\x4c\x46"

#define EI_CLASS 0x04
#define EI_CLASS_32BIT 1
#define EI_CLASS_64BIT 2
#define EI_DATA 0x05
#define EI_DATA_LE 1
#define EI_DATA_BE 2
#define EI_VERSION 0x06
#define EI_VERSION_ONLY_VALUE 1
#define EI_OSABI 0x07

#define ET_EXEC 0x02

#define EM_RISCV 0xf3

// e_machine values named by GNU readelf 2.47, the rest are unassigned.
// numbers get assigned over time, extend this when readelf learns new ones
static bool is_assigned_machine(u16 m)
{
    if (m > 0x103) {
        return false;
    }
    // unassigned gaps
    if ((m >= 0x0c && m <= 0x0e) || m == 0x10 || (m >= 0x18 && m <= 0x23) ||
        (m >= 0x79 && m <= 0x82) || m == 0x85 || m == 0x8f ||
        (m >= 0x91 && m <= 0x9f) || (m >= 0xce && m <= 0xd1) ||
        (m >= 0xe1 && m <= 0xf2)) {
        return false;
    }
    return true;
}

static enum ElfError validate_ident(const u8 ident[static EI_NIDENT])
{
    if (memcmp(ident, MAGIC, 4) != 0) {
        return ELF_BAD_MAGIC;
    }

    switch (ident[EI_CLASS]) {
        case EI_CLASS_32BIT:
            break;
        case EI_CLASS_64BIT:
            return ELF_UNSUPPORTED_CLASS;
        default:
            return ELF_BAD_CLASS;
    }

    switch (ident[EI_DATA]) {
        case EI_DATA_LE:
        case EI_DATA_BE:
            break;
        default:
            return ELF_BAD_ENDIAN;
    }

    if (ident[EI_VERSION] != EI_VERSION_ONLY_VALUE) {
        return ELF_BAD_IDENT_VERSION;
    }

    // 0x04 and 0x05 are unassigned, 0x40 and up are arch-specific and RISC-V
    // defines none
    const u8 ei_osabi = ident[EI_OSABI];
    if (ei_osabi == 0x04 || ei_osabi == 0x05 || ei_osabi > 0x12) {
        return ELF_BAD_OSABI;
    }

    // 0x08      we skip EI_ABIVERSION, no checks
    // 0x09-0x0F we skip 7 bytes of padding

    return ELF_OK;
}

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1])
{
    const enum ElfError err = validate_ident(hdr->e_ident);
    if (err != ELF_OK) {
        return err;
    }

    // 0x00-0x04 are defined, 0xfe00-0xfeff is OS-specific and 0xff00-0xffff
    // is processor-specific
    const u16 e_type = hdr->e_type;
    if (e_type > 0x04 && e_type < 0xfe00) {
        return ELF_BAD_TYPE;
    }
    if (e_type != ET_EXEC) {
        return ELF_UNSUPPORTED_TYPE;
    }

    if (!is_assigned_machine(hdr->e_machine)) {
        return ELF_BAD_MACHINE;
    }

    // supported targets only, add a case to support more
    switch (hdr->e_machine) {
        case EM_RISCV:
            break;
        default:
            return ELF_UNSUPPORTED_MACHINE;
    }

    if (hdr->e_version != 1) {
        return ELF_BAD_VERSION;
    }

    // e_entry, e_phoff, e_shoff are unchecked u32

    return ELF_OK;
}
