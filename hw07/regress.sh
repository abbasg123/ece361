#!/bin/sh
# Read each expected result, run balance, and compare output and exit status.
set -eu
cd "$(dirname "$0")"
work_dir=$(mktemp -d "${TMPDIR:-/tmp}/hw07-regress.XXXXXX")
trap 'rm -rf "$work_dir"' EXIT
trap 'exit 1' HUP INT TERM

total=0
failed=0
tab=$(printf '\t')

while IFS="$tab" read -r name expected_status expected_output
do
    total=$((total + 1))
    printf '%s\n' "$expected_output" > "$work_dir/expected.out"
    status=0
    if [ ! -f "inputs/$name.txt" ]
    then
        printf 'FAIL: %s (missing input)\n' "$name"
        failed=$((failed + 1))
        continue
    fi
    ./balance < "inputs/$name.txt" > "$work_dir/actual.out" 2> "$work_dir/actual.err" || status=$?

    if diff -u "$work_dir/expected.out" "$work_dir/actual.out" > "$work_dir/diff" &&
       [ "$status" = "$expected_status" ] &&
       [ ! -s "$work_dir/actual.err" ]
    then
        printf 'PASS: %s\n' "$name"
    else
        printf 'FAIL: %s (expected status %s, got %s)\n' "$name" "$expected_status" "$status"
        cat "$work_dir/diff"
        cat "$work_dir/actual.err"
        failed=$((failed + 1))
    fi
done < expected/results.tsv

if [ "$total" -lt 6 ]
then
    printf 'FAIL: expected at least six regression cases\n'
    failed=$((failed + 1))
fi
printf '%s regression tests; %s failed\n' "$total" "$failed"
[ "$failed" -eq 0 ]
