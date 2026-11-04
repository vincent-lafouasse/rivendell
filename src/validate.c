#include "validate.h"

#include <stdbool.h>
#include <string.h>

static const u8 k_magic[4] = {0x7f, 'E', 'L', 'F'};

// what the emulator can run: soft-float RV32I, no compressed instructions.
// RVE is a subset of RV32I, and harts run one at a time satisfy TSO
enum {
    RISCV_SUPPORTED_FLOAT_ABI = EF_RISCV_FLOAT_ABI_SOFT,
    // | EF_RISCV_RVC once C is implemented
    RISCV_SUPPORTED_FLAGS = EF_RISCV_RVE | EF_RISCV_TSO,
};

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

static enum RvdError validate_riscv_flags(u32 e_flags)
{
    if (e_flags & EF_RISCV_RESERVED) {
        return RVD_BAD_FLAGS;
    }
    if ((e_flags & EF_RISCV_FLOAT_ABI) != RISCV_SUPPORTED_FLOAT_ABI) {
        return RVD_UNSUPPORTED_FLAGS;
    }
    // everything else is a valid flag the emulator may not implement:
    // RVC, RV64ILP32, RVY and non-standard bits
    if (e_flags & ~(u32)(RISCV_SUPPORTED_FLAGS | EF_RISCV_FLOAT_ABI)) {
        return RVD_UNSUPPORTED_FLAGS;
    }
    return RVD_OK;
}

static enum RvdError validate_ident(const u8 ident[static EI_NIDENT])
{
    if (memcmp(ident, k_magic, sizeof k_magic) != 0) {
        return RVD_BAD_MAGIC;
    }

    switch (ident[EI_CLASS]) {
        case EI_CLASS_32BIT:
            break;
        case EI_CLASS_64BIT:
            return RVD_UNSUPPORTED_CLASS;
        default:
            return RVD_BAD_CLASS;
    }

    switch (ident[EI_DATA]) {
        case EI_DATA_LE:
        case EI_DATA_BE:
            break;
        default:
            return RVD_BAD_ENDIAN;
    }

    if (ident[EI_VERSION] != EI_VERSION_ONLY_VALUE) {
        return RVD_BAD_IDENT_VERSION;
    }

    // 0x04 and 0x05 are unassigned, 0x40 and up are arch-specific and RISC-V
    // defines none
    const u8 ei_osabi = ident[EI_OSABI];
    if (ei_osabi == 0x04 || ei_osabi == 0x05 || ei_osabi > 0x12) {
        return RVD_BAD_OSABI;
    }
    // the emulator runs bare-metal kernels, anything targeting an OS would
    // expect that OS's syscalls
    if (ei_osabi != ELFOSABI_NONE) {
        return RVD_UNSUPPORTED_OSABI;
    }

    // its meaning depends on EI_OSABI, ELFOSABI_NONE only defines 0
    if (ident[EI_ABIVERSION] != EI_ABIVERSION_NONE) {
        return RVD_UNSUPPORTED_ABI_VERSION;
    }

    // 0x09-0x0F we skip 7 bytes of padding, the spec says to ignore them

    return RVD_OK;
}

enum RvdError rvd_validate_ehdr(
    const struct Elf32_Ehdr hdr[static 1],
    size_t file_size
)
{
    const enum RvdError err = validate_ident(hdr->e_ident);
    if (err != RVD_OK) {
        return err;
    }

    // 0x00-0x04 are defined, 0xfe00-0xfeff is OS-specific and 0xff00-0xffff
    // is processor-specific
    const u16 e_type = hdr->e_type;
    if (e_type > 0x04 && e_type < 0xfe00) {
        return RVD_BAD_TYPE;
    }
    if (e_type != ET_EXEC) {
        // we want to load executables, this isn't one
        return RVD_UNSUPPORTED_TYPE;
    }

    if (hdr->e_version != EV_ONLY_VALUE) {
        return RVD_BAD_VERSION;
    }

    if (!is_assigned_machine(hdr->e_machine)) {
        return RVD_BAD_MACHINE;
    }

    if (hdr->e_ehsize != EHDR_SIZE) {
        return RVD_BAD_EHSIZE;
    }

    // supported targets only, add a case and a flags validator to support more
    enum RvdError flags_err;
    switch (hdr->e_machine) {
        case EM_RISCV:
            flags_err = validate_riscv_flags(hdr->e_flags);
            break;
        default:
            return RVD_UNSUPPORTED_MACHINE;
    }
    if (flags_err != RVD_OK) {
        return flags_err;
    }

    // e_entry, e_phoff, e_shoff are unchecked u32

    return RVD_OK;
}
