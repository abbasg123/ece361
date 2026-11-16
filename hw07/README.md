Build and test:

`make`
`./balance < inputs/balanced.txt`
`make test`
`make memcheck`
`make clean`

Bugs:

Destroy the same stack twice: 2nd destruction uses an array pointer that has already been freed

==25==ERROR: AddressSanitizer: heap-use-after-free on address 0x5030000003a0 at pc 0x557b5b7cdb69 bp 0x7ffcbb834dc0 sp 0x7ffcbb834db8



Pushing onto a destroyed stack: the push tries to use the 'count' of a freed structure
==45==ERROR: AddressSanitizer: heap-use-after-free on address 0x5030000003a8 at pc 0x55b232726c4a bp 0x7ffdc7abb990 sp 0x7ffdc7abb988

Not using istack_destroy in 'balance.c': structure and array stay allocated, leaked 24 bytes for structure and 32 bytes from array 
==42241==ERROR: LeakSanitizer: detected memory leaks
