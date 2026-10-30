#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "raw.h"

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

int main(int ac, char** av)
{
    (void)ac;
    (void)av;
    die("dummy");
}
