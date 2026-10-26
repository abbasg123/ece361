HW04: Tally
Counts how often each name occurs.

Run from hw04/:
- Build: make
- Run: printf 'door\ndoor\n' | ./events
- Test: make test
- Clean: make clean

using t->size without tally.c;
opaque_check.c:5:6: error: invalid use of incomplete typedef 'tally_t' {aka 'struct tally'}

tally_t members must be changed with library functions, as the fields are protected 