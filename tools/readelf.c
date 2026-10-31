#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "raw.h"

#define USAGE "Usage: rivendell_readelf binary.elf"

__attribute__((noreturn, format(printf, 1, 2))) static void die(const char* fmt,
                                                                ...)
{
    fflush(stdout);

    va_list ap;
    va_start(ap, fmt);
    fputs("raven: ", stderr);
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

int main(int ac, char** av)
{
    if (ac != 2) {
        die(USAGE);
    }

    const char* const elf_path = av[1];
}
