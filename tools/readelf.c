#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "raw.h"

#define USAGE "Usage: rivendell_readelf [--file-header|--segments] binary.elf"

__attribute__((noreturn, format(printf, 1, 2))) static void die(
    const char* fmt,
    ...
)
{
    fflush(stdout);

    va_list ap;
    va_start(ap, fmt);
    fputs("rivendell_readelf: ", stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(EXIT_FAILURE);
}

struct Config {
    const char* const path;
    bool header;
    bool segments;
};

static struct Config parse_config_or_die(int ac, const char* const* const av)
{
    if (ac == 2) {
        return (struct Config){
            .path = av[1],
            .header = true,
            .segments = true,
        };
    }

    if (ac != 3) {
        die(USAGE);
    }

    // ac == 3
    const char* const flag = av[1];
    const char* const path = av[2];

    const bool header = strcmp(flag, "--file-header") == 0;
    const bool segments = strcmp(flag, "--segments") == 0;

    if (!header && !segments) {
        die(USAGE);
    }

    return (struct Config){
        .path = path,
        .header = header,
        .segments = segments,
    };
}

static size_t file_size(FILE* f)
{
    fseek(f, 0L, SEEK_END);
    const size_t sz = (size_t)ftell(f);
    rewind(f);
    return sz;
}

struct Buffer {
    u8* data;
    size_t sz;
};

static struct Buffer read_file_or_die(const char* const path)
{
    FILE* f = fopen(path, "rb");
    if (!f) {
        die("Failed to open %s", path);
    }

    const size_t sz = file_size(f);
    if (sz == 0) {
        die("Empty file");
    }
    u8* data = malloc(sz);
    if (!data) {
        die("Out of memory");
    }

    if (fread(data, 1, sz, f) != sz) {
        die("Failed to read %s", path);
    }
    fclose(f);

    return (struct Buffer){.data = data, .sz = sz};
}

// names and layout follow GNU readelf, so `readelf -h` output can be diffed.
// only values that pass validation are printed, the fallbacks are for safety
static const char* class_name(u8 ei_class)
{
    switch (ei_class) {
        case EI_CLASS_32BIT:
            return "ELF32";
        case EI_CLASS_64BIT:
            return "ELF64";
        default:
            return "<unknown>";
    }
}

static const char* data_name(u8 ei_data)
{
    switch (ei_data) {
        case EI_DATA_LE:
            return "2's complement, little endian";
        case EI_DATA_BE:
            return "2's complement, big endian";
        default:
            return "<unknown>";
    }
}

static const char* osabi_name(u8 ei_osabi)
{
    switch (ei_osabi) {
        case ELFOSABI_NONE:
            return "UNIX - System V";
        default:
            return "<unknown>";
    }
}

static const char* type_name(u16 e_type)
{
    switch (e_type) {
        case ET_EXEC:
            return "EXEC (Executable file)";
        default:
            return "<unknown>";
    }
}

static const char* machine_name(u16 e_machine)
{
    switch (e_machine) {
        case EM_RISCV:
            return "RISC-V";
        default:
            return "<unknown>";
    }
}

static const char* riscv_float_abi_name(u32 e_flags)
{
    switch (e_flags & EF_RISCV_FLOAT_ABI) {
        case EF_RISCV_FLOAT_ABI_SOFT:
            return "soft-float ABI";
        case EF_RISCV_FLOAT_ABI_SINGLE:
            return "single-float ABI";
        case EF_RISCV_FLOAT_ABI_DOUBLE:
            return "double-float ABI";
        case EF_RISCV_FLOAT_ABI_QUAD:
            return "quad-float ABI";
        default:
            return "<unknown>";
    }
}

// e.g. "0x18, RVE, TSO, soft-float ABI", GNU only decodes non-zero flags
static void print_riscv_flags(u32 e_flags)
{
    printf("0x%x", e_flags);
    if (e_flags == 0) {
        return;
    }
    if (e_flags & EF_RISCV_RVC) {
        printf(", RVC");
    }
    if (e_flags & EF_RISCV_RVE) {
        printf(", RVE");
    }
    if (e_flags & EF_RISCV_TSO) {
        printf(", TSO");
    }
    printf(", %s", riscv_float_abi_name(e_flags));
}

#define FIELD "  %-35s"

static void print_file_header(const struct Elf32_Ehdr hdr[static 1])
{
    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (size_t i = 0; i < EI_NIDENT; i++) {
        printf("%02x ", hdr->e_ident[i]);
    }
    printf("\n");

    const u8 ei_version = hdr->e_ident[EI_VERSION];
    printf(FIELD "%s\n", "Class:", class_name(hdr->e_ident[EI_CLASS]));
    printf(FIELD "%s\n", "Data:", data_name(hdr->e_ident[EI_DATA]));
    printf(
        FIELD "%u%s\n",
        "Version:",
        ei_version,
        ei_version == EI_VERSION_ONLY_VALUE ? " (current)" : ""
    );
    printf(FIELD "%s\n", "OS/ABI:", osabi_name(hdr->e_ident[EI_OSABI]));
    printf(FIELD "%u\n", "ABI Version:", hdr->e_ident[EI_ABIVERSION]);
    printf(FIELD "%s\n", "Type:", type_name(hdr->e_type));
    printf(FIELD "%s\n", "Machine:", machine_name(hdr->e_machine));
    printf(FIELD "0x%x\n", "Version:", hdr->e_version);
    printf(FIELD "0x%x\n", "Entry point address:", hdr->e_entry);
    printf(
        FIELD "%u (bytes into file)\n",
        "Start of program headers:",
        hdr->e_phoff
    );
    printf(
        FIELD "%u (bytes into file)\n",
        "Start of section headers:",
        hdr->e_shoff
    );
    printf(FIELD, "Flags:");
    print_riscv_flags(hdr->e_flags);  // validation only lets RISC-V through
    printf("\n");
    printf(FIELD "%u (bytes)\n", "Size of this header:", hdr->e_ehsize);
    printf(FIELD "%u (bytes)\n", "Size of program headers:", hdr->e_phentsize);
    printf(FIELD "%u\n", "Number of program headers:", hdr->e_phnum);
    printf(FIELD "%u (bytes)\n", "Size of section headers:", hdr->e_shentsize);
    printf(FIELD "%u\n", "Number of section headers:", hdr->e_shnum);
    printf(FIELD "%u\n", "Section header string table index:", hdr->e_shstrndx);
}

int main(int ac, const char* const* const av)
{
    const struct Config cfg = parse_config_or_die(ac, av);

    const struct Buffer bytes = read_file_or_die(cfg.path);

    // validate first, the printers only have to handle supported files
    struct Header32 header;
    const enum RvdError err = rvd_parse_header(bytes.data, bytes.sz, &header);
    if (err != RVD_OK) {
        die("Validation error: %s", rvd_error_str(err));
    }

    if (cfg.header) {
        const struct Elf32_Ehdr raw = rvd_read_ehdr(bytes.data, header.endian);
        print_file_header(&raw);
    }
    if (cfg.segments) {
        die("segments are yet to be supported");
    }

    free(bytes.data);
}
