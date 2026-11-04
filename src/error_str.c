#include <rivendell/error.h>

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

const char* rvd_error_kind_str(enum RvdErrorKind kind)
{
    switch (kind) {
        case RVD_OK:
            return "ok";
        case RVD_TRUNCATED:
            return "file is truncated";
        case RVD_BAD_MAGIC:
            return "bad ELF magic";
        case RVD_BAD_CLASS:
            return "invalid ELF class";
        case RVD_UNSUPPORTED_CLASS:
            return "unsupported ELF class (expected 32-bit)";
        case RVD_BAD_ENDIAN:
            return "invalid ELF data encoding";
        case RVD_BAD_IDENT_VERSION:
            return "unsupported e_ident version";
        case RVD_BAD_OSABI:
            return "invalid OS/ABI";
        case RVD_UNSUPPORTED_OSABI:
            return "unsupported OS/ABI (expected none/SYSV)";
        case RVD_UNSUPPORTED_ABI_VERSION:
            return "unsupported ABI version (expected 0)";
        case RVD_BAD_TYPE:
            return "invalid ELF type";
        case RVD_UNSUPPORTED_TYPE:
            return "unsupported ELF type (expected executable)";
        case RVD_BAD_MACHINE:
            return "invalid machine";
        case RVD_UNSUPPORTED_MACHINE:
            return "unsupported machine";
        case RVD_BAD_VERSION:
            return "unsupported ELF version";
        case RVD_BAD_FLAGS:
            return "invalid processor flags";
        case RVD_UNSUPPORTED_FLAGS:
            return "unsupported processor flags";
        case RVD_BAD_EHSIZE:
            return "bad ELF header size";
        case RVD_BAD_PHENTSIZE:
            return "bad program header entry size";
        case RVD_NO_SEGMENTS:
            return "no program headers";
        case RVD_UNSUPPORTED_PHNUM:
            return "unsupported program header count (PN_XNUM)";
        case RVD_PHDRS_OUT_OF_FILE:
            return "program header table extends past end of file";
        case RVD_MISALIGNED_ENTRY:
            return "entry point is misaligned";
        case RVD_BAD_ENTRY:
            return "entry point is not in a loaded segment";
        case RVD_SEGMENT_OUT_OF_FILE:
            return "segment extends past end of file";
        case RVD_SEGMENT_OUT_OF_RAM:
            return "segment does not fit in RAM";
        case RVD_SEGMENT_FILESZ_GT_MEMSZ:
            return "segment file size exceeds its memory size";
        case RVD_SEGMENT_BAD_ALIGN:
            return "segment alignment is invalid";
    }
    return "unknown error";
}

enum ValueBase {
    VALUE_NONE,
    VALUE_DEC,
    VALUE_HEX,
};

struct ValueFormat {
    const char* label;
    enum ValueBase base;
};

// keep in sync with the table in rivendell/error.h. sizes, offsets and
// indices are decimal, raw fields are hex like in readelf
static struct ValueFormat value_format(enum RvdErrorKind kind)
{
    switch (kind) {
        case RVD_TRUNCATED:
            return (struct ValueFormat){"file size", VALUE_DEC};
        case RVD_BAD_MAGIC:
            return (struct ValueFormat){"magic", VALUE_HEX};
        case RVD_BAD_CLASS:
        case RVD_UNSUPPORTED_CLASS:
            return (struct ValueFormat){"EI_CLASS", VALUE_HEX};
        case RVD_BAD_ENDIAN:
            return (struct ValueFormat){"EI_DATA", VALUE_HEX};
        case RVD_BAD_IDENT_VERSION:
            return (struct ValueFormat){"EI_VERSION", VALUE_HEX};
        case RVD_BAD_OSABI:
        case RVD_UNSUPPORTED_OSABI:
            return (struct ValueFormat){"EI_OSABI", VALUE_HEX};
        case RVD_UNSUPPORTED_ABI_VERSION:
            return (struct ValueFormat){"EI_ABIVERSION", VALUE_HEX};
        case RVD_BAD_TYPE:
        case RVD_UNSUPPORTED_TYPE:
            return (struct ValueFormat){"e_type", VALUE_HEX};
        case RVD_BAD_MACHINE:
        case RVD_UNSUPPORTED_MACHINE:
            return (struct ValueFormat){"e_machine", VALUE_HEX};
        case RVD_BAD_VERSION:
            return (struct ValueFormat){"e_version", VALUE_HEX};
        case RVD_BAD_FLAGS:
        case RVD_UNSUPPORTED_FLAGS:
            return (struct ValueFormat){"e_flags", VALUE_HEX};
        case RVD_BAD_EHSIZE:
            return (struct ValueFormat){"e_ehsize", VALUE_DEC};
        case RVD_BAD_PHENTSIZE:
            return (struct ValueFormat){"e_phentsize", VALUE_DEC};
        case RVD_UNSUPPORTED_PHNUM:
            return (struct ValueFormat){"e_phnum", VALUE_DEC};
        case RVD_PHDRS_OUT_OF_FILE:
            return (struct ValueFormat){"table end", VALUE_DEC};
        case RVD_MISALIGNED_ENTRY:
        case RVD_BAD_ENTRY:
            return (struct ValueFormat){"e_entry", VALUE_HEX};
        case RVD_SEGMENT_OUT_OF_FILE:
        case RVD_SEGMENT_OUT_OF_RAM:
        case RVD_SEGMENT_FILESZ_GT_MEMSZ:
        case RVD_SEGMENT_BAD_ALIGN:
            return (struct ValueFormat){"segment", VALUE_DEC};
        case RVD_OK:
        case RVD_NO_SEGMENTS:
            break;
    }
    return (struct ValueFormat){"", VALUE_NONE};
}

// the bound: longest kind string (46) + " (" + longest label (13) + " " +
// longest u64 (20 decimal digits) + ")" + NUL = 84
char* rvd_error_format(
    struct RvdError e,
    char buffer[static RVD_ERROR_FORMAT_MAX]
)
{
    const char* const kind = rvd_error_kind_str(e.kind);
    const struct ValueFormat fmt = value_format(e.kind);

    int n = 0;
    switch (fmt.base) {
        case VALUE_NONE:
            n = snprintf(buffer, RVD_ERROR_FORMAT_MAX, "%s", kind);
            break;
        case VALUE_DEC:
            n = snprintf(
                buffer,
                RVD_ERROR_FORMAT_MAX,
                "%s (%s %" PRIu64 ")",
                kind,
                fmt.label,
                e.value
            );
            break;
        case VALUE_HEX:
            n = snprintf(
                buffer,
                RVD_ERROR_FORMAT_MAX,
                "%s (%s 0x%" PRIx64 ")",
                kind,
                fmt.label,
                e.value
            );
            break;
    }
    assert(n >= 0 && n < RVD_ERROR_FORMAT_MAX);
    (void)n;
    return buffer;
}
