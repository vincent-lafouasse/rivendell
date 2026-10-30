#include "validate.h"

#include <string.h>

#include "debug.h"

#define MAGIC "\x7f\x45\x4c\x46"

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1])
{
    const u8* const e_ident = hdr->e_ident;

    if (memcmp(e_ident, MAGIC, 4) != 0) {
        return ELF_BAD_MAGIC;
    }

    const u8 ei_class = e_ident[0x04];
    switch (ei_class) {
        case 1:
        case 2:
            break;
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

    return ELF_OK;
}
