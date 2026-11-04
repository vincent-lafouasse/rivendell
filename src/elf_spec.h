#pragma once

#define EI_NIDENT 16

// e_ident byte indices
enum EiIndex {
    EI_CLASS = 0x04,
    EI_DATA = 0x05,
    EI_VERSION = 0x06,
    EI_OSABI = 0x07,
    EI_ABIVERSION = 0x08,
};

// e_ident[EI_CLASS]
enum EiClass {
    EI_CLASS_32BIT = 1,
    EI_CLASS_64BIT = 2,
};

// e_ident[EI_DATA]
enum EiData {
    EI_DATA_LE = 1,
    EI_DATA_BE = 2,
};

// e_ident[EI_OSABI] and e_ident[EI_ABIVERSION], only what's used so far
enum EiOsAbi {
    ELFOSABI_NONE = 0,  // no OS extensions, a.k.a. SYSV
    EI_ABIVERSION_NONE = 0,
};

// e_ident[EI_VERSION] and e_version, the spec only defines 1
enum ElfVersion {
    EI_VERSION_ONLY_VALUE = 1,
    EV_ONLY_VALUE = 1,
};

// e_type
enum ElfType {
    ET_EXEC = 0x02,
};

// e_machine
enum ElfMachine {
    EM_RISCV = 0xf3,
};

// RISC-V e_flags, from the RISC-V ELF psABI, section "File Header":
// https://github.com/riscv-non-isa/riscv-elf-psabi-doc/blob/master/riscv-elf.adoc
enum RiscvFlags {
    EF_RISCV_RVC = 0x00000001,
    EF_RISCV_FLOAT_ABI = 0x00000006,  // mask for the field below
    EF_RISCV_RVE = 0x00000008,
    EF_RISCV_TSO = 0x00000010,
    EF_RISCV_RV64ILP32 = 0x00000020,  // experimental, RV64 hardware only
    EF_RISCV_RVY = 0x00000040,
    EF_RISCV_RESERVED = 0x00ffff80,  // bits 7-23, must not be set
};

// values of the EF_RISCV_FLOAT_ABI field
enum RiscvFloatAbi {
    EF_RISCV_FLOAT_ABI_SOFT = 0x00000000,
    EF_RISCV_FLOAT_ABI_SINGLE = 0x00000002,
    EF_RISCV_FLOAT_ABI_DOUBLE = 0x00000004,
    EF_RISCV_FLOAT_ABI_QUAD = 0x00000006,
};

// bits 24-31 are toolchain-defined. a macro since it's too big for an int
// enumerator in C99
#define EF_RISCV_NONSTANDARD 0xff000000u

// e_phnum escape value, the real count is in section header 0's sh_info
enum ElfPhnum {
    PN_XNUM = 0xffff,
};

// on-disk size of Elf32 headers
enum Elf32Size {
    EHDR_SIZE = 52,
    PHDR_SIZE = 32,
    SHDR_SIZE = 40,
};
