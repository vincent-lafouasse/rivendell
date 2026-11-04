#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "header.h"

#define USAGE "Usage: rivendell_readelf binary.elf"

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

int main(int ac, char** av)
{
    if (ac != 2) {
        die(USAGE);
    }

    const char* const elf_path = av[1];
    const struct Buffer bytes = read_file_or_die(elf_path);

    struct Header32 header; // dummy, just meant to trigger full validation of Ehdr
    const enum RvdError err = rvd_parse_header(bytes.data, bytes.sz, &header);
    if (err != RVD_OK) {
        die("Validation error: %s", rvd_error_str(err));
    }

    free(bytes.data);
}
