The library prints, extracts, replaces, and sign extends user defined fields to decode a 16 bit thermostat status word.

make compiles, make test builds and runs tests, make clean removes build outputs. 

words and values are uint32_t, thermostat words are uint16_t, width can be 1-32, position can be 0-31. Fields require a position + bit less than or equal to 32. 

width 32 uses the whole word, requires position of 0. Position 31 allows only width 1. Oversized replacement values are truncated. sinle bit sign extension returns 0 or -1, 32 bit sign extension ranges from
-2147483648 to 2147483647

invalid width/position:
printing produces no output
extraction and replacement return the original word
sign extension returns 0

invalid mode:
modes 5-7 result in an invalid mode bool on modeValidity. Modes 0-4 are accepted and result in a true, valid mode. res is a reserved bit and reports a reserve bit violation if set.