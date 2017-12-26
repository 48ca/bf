# Brainfuck interpreter
`bf.cc` is a fast brainfuck interpreter that can execute most brainfuck files as intended. All operations after reading are executed in O(1) average time.

`bf.c` is a C brainfuck interpreter, written first, and much less configurable. It is still fast due to its low overhead, but many operations are still O(n) in time.

These interpreters have no special behavior when reading EOFs from `stdin` (due to executing a ','), and the data arrays for both contain 8-bit values. There are checks to prevent integer underflow on the data offset pointer (index of the vector).

`texttobf.cc` is a very basic program that converts text into executable brainfuck code that will output the text that was inputted.
