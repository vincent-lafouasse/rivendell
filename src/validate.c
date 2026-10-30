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

// RISC-V e_flags, from the RISC-V ELF psABI, section "File Header":
// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-elf.adoc
#define EF_RISCV_RVC 0x00000001
#define EF_RISCV_FLOAT_ABI 0x00000006  // 0 soft, 2 single, 4 double, 6 quad
#define EF_RISCV_RVE 0x00000008
#define EF_RISCV_TSO 0x00000010
#define EF_RISCV_RV64ILP32 0x00000020  // experimental, RV64 hardware only
#define EF_RISCV_RVY 0x00000040
#define EF_RISCV_RESERVED 0x00ffff80     // bits 7-23, must not be set
#define EF_RISCV_NONSTANDARD 0xff000000  // bits 24-31, toolchain-defined

// what the emulator can run: soft-float RV32I, no compressed instructions.
// add EF_RISCV_RVC here once C is implemented
#define RISCV_SUPPORTED_FLAGS 0x00000000

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

static enum ElfError validate_riscv_flags(u32 e_flags)
{
    if (e_flags & EF_RISCV_RESERVED) {
        return ELF_BAD_FLAGS;
    }
    // everything else is a valid flag the emulator may not implement:
    // hard-float ABIs, RVE, TSO, RV64ILP32, RVY and non-standard bits
    if (e_flags & ~(u32)RISCV_SUPPORTED_FLAGS) {
        return ELF_UNSUPPORTED_FLAGS;
    }
    return ELF_OK;
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
