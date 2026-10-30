#include "raw.h"

struct Elf32_Ehdr read_elf_header(const uint8_t data[static 52]);

struct Elf32_Phdr read_program_header(const uint8_t data[static 32]);
