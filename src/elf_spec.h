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

// e_ident[EI_OSABI] and e_ident[EI_ABIVERSION], only what we support
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

// on-disk size of an Elf32 section header
// raw.h
enum Elf32Size {
    EHDR_SIZE = 52,
    PHDR_SIZE = 32,
    SHDR_SIZE = 40,
};
