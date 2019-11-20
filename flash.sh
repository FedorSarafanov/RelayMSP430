#!/usr/bin/env bash

rm -f balloon.bin

msp430-gcc  -Os -mmcu=msp430f5529 -o balloon.bin balloon.c

mspdebug tilib "prog balloon.bin"
 
