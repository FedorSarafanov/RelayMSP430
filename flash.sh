#!/bin/bash

rm -f *.bin
msp430-gcc  -Os -mmcu=msp430f5529 --save-temps -o firmware.bin relay.c 
mspdebug tilib "prog firmware.bin"
