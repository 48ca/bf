#!/bin/bash -eux

DIR=$(dirname ${BASH_SOURCE[0]})
cat $DIR/examples/text/mobydick.txt | $DIR/texttobf | $DIR/brainfuck -d 1000000 - | less
