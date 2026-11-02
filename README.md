# rivendell

An ELF loader library for `raven`. Its job is to validate an executable and
copy its loadable segments into the emulator's RAM, so the emulator can boot a
bare-metal image such as an RV32 port of xv6.

Status: header validation works, segment loading is in progress.

## Supported files

Anything else is rejected with an `RVD_BAD_*` (not valid ELF) or
`RVD_UNSUPPORTED_*` (valid, but not handled) error.

| Field          | Accepted                                       |
| -------------- | ---------------------------------------------- |
| Class          | 32-bit (`ELFCLASS32`)                          |
| OS/ABI         | none/SYSV, ABI version 0                       |
| Type           | executable (`ET_EXEC`)                         |
| Machine        | RISC-V (`EM_RISCV`)                            |
| RISC-V e_flags | soft-float, no RVC, RVE, TSO or other features |

Not supported: 64-bit files, object files, shared objects and PIE, dynamic
linking.

## Roadmap

- MVP: RV32
- post-MVP: 32-bit ARM, e.g. STM32F4 firmware
- v2: 64-bit ELF and ARM64
