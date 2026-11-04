#include "header.h"

#include "validate.h"

enum RvdError rvd_parse_header(
    const u8* file,
    size_t file_size,
    struct Header32 out[static 1]
)
{
    if (file_size < EHDR_SIZE) {
        return RVD_TRUNCATED;
    }

    const enum Endianness endian = rvd_parse_endianness(file[EI_DATA]);

    const struct Elf32_Ehdr raw = rvd_read_ehdr(file, endian);

    const enum RvdError validation_err = rvd_validate_ehdr(&raw, file_size);
    if (validation_err != RVD_OK) {
        return validation_err;
    }

    *out = (struct Header32){
        .endian = endian,
        .entry = raw.e_entry,
        .ph_offset = raw.e_phoff,
        .ph_num = raw.e_phnum,
    };
    return RVD_OK;
}
