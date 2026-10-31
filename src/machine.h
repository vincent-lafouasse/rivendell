#pragma once

#include "types.h"

enum Machine { DUMMY_ };

enum RiscvFloatAbi { DUMMY };

struct RiscvFlags {
    u32 dummy;
};

struct MachineInfo {
    enum Machine machine;  // MACHINE_RISCV for now
    union {
        struct RiscvFlags riscv;
        // struct ArmFlags arm;  // post-MVP
    } as;
};
