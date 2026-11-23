Run from hw08/:

make
make test
make memcheck
make clean

make builds both programs; 
make test runs the tests; 
make memcheck runs them with the sanitizers; 
make clean removes the executables.

Search Crash:
Trigger: test_empty_tree in tests/test_bst.c searches for 27 immediately after creating an empty tree.
bst.c:124:19: runtime error: member access within null pointer of type 'const struct node'

The original main inserts seven keys before either search, so it never searches an empty tree.

Increasing insertion order creates a chain of right children, giving height 1023 and allowing a search to examine all 1023 nodes. Shuffling produced height 21 in this run, so a search can examine at most 21 nodes; the shuffled height varies between runs.