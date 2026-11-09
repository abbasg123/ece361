#!/bin/sh
set -u

CDPATH= cd -- "$(dirname -- "$0")" || exit 1
regress_tmp=$(mktemp -d) || exit 1
trap 'rm -rf "$regress_tmp"' 0
trap 'exit 1' HUP INT TERM

passed=0
failed=0
for scenario in scenarios/*.txt
do
    [ -f "$scenario" ] || continue
    name=${scenario##*/}

    if ! ./replay "$scenario" > "$regress_tmp/$name"
    then
        printf 'FAIL: %s (replay failed)\n' "$name"
        failed=$((failed + 1))
    elif diff -u "expected/$name" "$regress_tmp/$name"
    then
        printf 'PASS: %s\n' "$name"
        passed=$((passed + 1))
    else
        printf 'FAIL: %s (trace differs)\n' "$name"
        failed=$((failed + 1))
    fi
done

if [ "$passed" -eq 0 ] && [ "$failed" -eq 0 ]
then
    printf 'FAIL: no regression scenarios found\n'
    exit 1
fi

printf '%d regression tests passed, %d failed\n' "$passed" "$failed"
[ "$failed" -eq 0 ]
