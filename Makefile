all:
	msp430-elf-gcc -I /opt/ti/msp430-gcc/include -Os -mmcu=msp430f5529 -o relay.bin relay.c
	mspdebug tilib "prog relay.bin"