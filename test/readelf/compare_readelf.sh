#!/usr/bin/env bash
# diffs `rivendell_readelf --file-header` against GNU `readelf -h` on every
# test/*.elf. usage: compare_readelf.sh path/to/rivendell_readelf test_dir

# portability be damned
READELF=${READELF:-/opt/homebrew/opt/binutils/bin/readelf}

# ctest reports exit code 77 as skipped
SKIP=77

if [ $# -ne 2 ]; then
    echo "usage: $0 path/to/rivendell_readelf test_dir" >&2
    exit 2
fi
rivendell_readelf=$1
test_dir=$2

if [ ! -x "$READELF" ]; then
    echo "GNU readelf not found at $READELF, set READELF to override" >&2
    exit $SKIP
fi

failed=0
for elf in "$test_dir"/*.elf; do
    if diff -u \
        <("$READELF" --file-header "$elf" 2>&1) \
        <("$rivendell_readelf" --file-header "$elf" 2>&1); then
        echo "ok   $(basename "$elf")"
    else
        echo "FAIL $(basename "$elf")"
        failed=1
    fi
done
exit $failed
