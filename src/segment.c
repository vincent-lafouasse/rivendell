#include "segment.h"

struct Elf32_Phdr rvd_read_phdr_at(
    const u8* file,
    const struct Header32 hdr[static 1],
    size_t i
)
{
    const size_t offset = hdr->ph_offset + i * PHDR_SIZE;

    return rvd_read_phdr(file + offset, hdr->endian);
}
