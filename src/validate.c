#include "validate.h"

#include <stdbool.h>
#include <string.h>

#include "debug.h"

#define MAGIC "\x7f\x45\x4c\x46"

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

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1])
{
    const u8* const e_ident = hdr->e_ident;

    if (memcmp(e_ident, MAGIC, 4) != 0) {
        return ELF_BAD_MAGIC;
    }

    const u8 ei_class = e_ident[0x04];
    switch (ei_class) {
        case 1:
            // 32 bit
            break;
        case 2:
            // 64 bit
            return ELF_UNSUPPORTED_CLASS;
        default:
            return ELF_BAD_CLASS;
    }

    // already checked
    const u8 ei_data = e_ident[0x05];
    switch (ei_data) {
        case 1:
        case 2:
            break;
        default:
            UNREACHABLE();
    }

    const u8 ei_version = e_ident[0x06];
    if (ei_version != 1) {
        return ELF_BAD_IDENT_VERSION;
    }

    const u8 ei_osabi = e_ident[0x07];
    if (ei_osabi == 0x05 || ei_osabi > 0x12) {
        return ELF_BAD_OSABI;
    }

    // 0x08      we skip EI_ABIVERSION, no checks
    // 0x09-0x0A we skip 7 bytes of padding

    switch (hdr->e_type) {
        case 0x0:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0xfe00:
        case 0xfeff:
        case 0xff00:
        case 0xffff:
            break;
        default:
            return ELF_BAD_TYPE;
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
