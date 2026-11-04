#include "validate.h"

#include <stdbool.h>
#include <string.h>

#include "err.h"

static const u8 k_magic[4] = {0x7f, 'E', 'L', 'F'};

// what the emulator can run: soft-float RV32I, no compressed instructions.
// RVE is a subset of RV32I, and harts run one at a time satisfy TSO
enum {
    RISCV_SUPPORTED_FLOAT_ABI = EF_RISCV_FLOAT_ABI_SOFT,
    // | EF_RISCV_RVC once C is implemented
    RISCV_SUPPORTED_FLAGS = EF_RISCV_RVE | EF_RISCV_TSO,
    // 2 once C is implemented
    RISCV_ENTRY_ALIGN = 4,
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

static struct RvdError validate_riscv_flags(u32 e_flags)
{
    if (e_flags & EF_RISCV_RESERVED) {
        return rvd_err(RVD_BAD_FLAGS, e_flags);
    }
    if ((e_flags & EF_RISCV_FLOAT_ABI) != RISCV_SUPPORTED_FLOAT_ABI) {
        return rvd_err(RVD_UNSUPPORTED_FLAGS, e_flags);
    }
    // everything else is a valid flag the emulator may not implement:
    // RVC, RV64ILP32, RVY and non-standard bits
    if (e_flags & ~(u32)(RISCV_SUPPORTED_FLAGS | EF_RISCV_FLOAT_ABI)) {
        return rvd_err(RVD_UNSUPPORTED_FLAGS, e_flags);
    }
    return rvd_ok();
}

static struct RvdError validate_riscv(const struct Elf32_Ehdr hdr[static 1])
{
    const struct RvdError err = validate_riscv_flags(hdr->e_flags);
    if (rvd_is_err(err)) {
        return err;
    }
    // the first fetch would raise an instruction-address-misaligned exception.
    // whether the entry point is actually loaded is checked with the segments
    if (hdr->e_entry % RISCV_ENTRY_ALIGN != 0) {
        return rvd_err(RVD_MISALIGNED_ENTRY, hdr->e_entry);
    }
    return rvd_ok();
}

static struct RvdError validate_ident(const u8 ident[static EI_NIDENT])
{
    if (memcmp(ident, k_magic, sizeof k_magic) != 0) {
        const u64 magic = (u64)ident[0] << 24 | (u64)ident[1] << 16 |
                          (u64)ident[2] << 8 | (u64)ident[3];
        return rvd_err(RVD_BAD_MAGIC, magic);
    }

    switch (ident[EI_CLASS]) {
        case EI_CLASS_32BIT:
            break;
        case EI_CLASS_64BIT:
            return rvd_err(RVD_UNSUPPORTED_CLASS, ident[EI_CLASS]);
        default:
            return rvd_err(RVD_BAD_CLASS, ident[EI_CLASS]);
    }

    switch (ident[EI_DATA]) {
        case EI_DATA_LE:
        case EI_DATA_BE:
            break;
        default:
            return rvd_err(RVD_BAD_ENDIAN, ident[EI_DATA]);
    }

    if (ident[EI_VERSION] != EI_VERSION_ONLY_VALUE) {
        return rvd_err(RVD_BAD_IDENT_VERSION, ident[EI_VERSION]);
    }

    // 0x04 and 0x05 are unassigned, 0x40 and up are arch-specific and RISC-V
    // defines none
    const u8 ei_osabi = ident[EI_OSABI];
    if (ei_osabi == 0x04 || ei_osabi == 0x05 || ei_osabi > 0x12) {
        return rvd_err(RVD_BAD_OSABI, ei_osabi);
    }
    // the emulator runs bare-metal kernels, anything targeting an OS would
    // expect that OS's syscalls
    if (ei_osabi != ELFOSABI_NONE) {
        return rvd_err(RVD_UNSUPPORTED_OSABI, ei_osabi);
    }

    // its meaning depends on EI_OSABI, ELFOSABI_NONE only defines 0
    if (ident[EI_ABIVERSION] != EI_ABIVERSION_NONE) {
        return rvd_err(RVD_UNSUPPORTED_ABI_VERSION, ident[EI_ABIVERSION]);
    }

    // 0x09-0x0F we skip 7 bytes of padding, the spec says to ignore them

    return rvd_ok();
}

struct RvdError rvd_validate_ehdr(
    const struct Elf32_Ehdr hdr[static 1],
    size_t file_size
)
{
    const struct RvdError err = validate_ident(hdr->e_ident);
    if (rvd_is_err(err)) {
        return err;
    }

    // 0x00-0x04 are defined, 0xfe00-0xfeff is OS-specific and 0xff00-0xffff
    // is processor-specific
    const u16 e_type = hdr->e_type;
    if (e_type > 0x04 && e_type < 0xfe00) {
        return rvd_err(RVD_BAD_TYPE, e_type);
    }
    if (e_type != ET_EXEC) {
        // we want to load executables, this isn't one
        return rvd_err(RVD_UNSUPPORTED_TYPE, e_type);
    }

    if (hdr->e_version != EV_ONLY_VALUE) {
        return rvd_err(RVD_BAD_VERSION, hdr->e_version);
    }

    if (!is_assigned_machine(hdr->e_machine)) {
        return rvd_err(RVD_BAD_MACHINE, hdr->e_machine);
    }

    if (hdr->e_ehsize != EHDR_SIZE) {
        return rvd_err(RVD_BAD_EHSIZE, hdr->e_ehsize);
    }

    // supported targets only, add a case and a validator to support more
    struct RvdError machine_err;
    switch (hdr->e_machine) {
        case EM_RISCV:
            machine_err = validate_riscv(hdr);
            break;
        default:
            return rvd_err(RVD_UNSUPPORTED_MACHINE, hdr->e_machine);
    }
    if (rvd_is_err(machine_err)) {
        return machine_err;
    }

    // an executable without program headers has nothing to load
    if (hdr->e_phnum == 0) {
        return rvd_err(RVD_NO_SEGMENTS, 0);
    }
    // the real count would be in section header 0, which we don't read
    if (hdr->e_phnum == PN_XNUM) {
        return rvd_err(RVD_UNSUPPORTED_PHNUM, hdr->e_phnum);
    }
    if (hdr->e_phentsize != PHDR_SIZE) {
        return rvd_err(RVD_BAD_PHENTSIZE, hdr->e_phentsize);
    }
    // u64 so that a huge e_phoff can't wrap around
    const u64 ph_end = (u64)hdr->e_phoff + (u64)hdr->e_phnum * PHDR_SIZE;
    if (ph_end > file_size) {
        return rvd_err(RVD_PHDRS_OUT_OF_FILE, ph_end);
    }

    // e_sh* are ignored

    return rvd_ok();
}
