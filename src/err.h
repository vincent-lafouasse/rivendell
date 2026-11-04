#pragma once

#include <stdbool.h>

#include <rivendell/error.h>

#include "types.h"

static inline struct RvdError rvd_ok(void)
{
    return (struct RvdError){.kind = RVD_OK, .value = 0};
}

static inline struct RvdError rvd_err(enum RvdErrorKind kind, u64 value)
{
    return (struct RvdError){.kind = kind, .value = value};
}

static inline bool rvd_is_err(struct RvdError e)
{
    return e.kind != RVD_OK;
}
