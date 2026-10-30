#include "validate.h"

#include <string.h>

#define MAGIC "\x7f\x45\x4c\x46"

enum ElfError validate_ehdr(const struct Elf32_Ehdr hdr[static 1])
{
    if (memcmp(hdr->e_ident, MAGIC, 4) != 0) {
        return ELF_BAD_MAGIC;
    }

    return ELF_OK;
}
