all:
	msp430-elf-gcc -I /opt/ti/msp430-gcc/include -Os -mmcu=msp430f5529 -o balloon.bin balloon.c
	mspdebug tilib "prog balloon.bin"