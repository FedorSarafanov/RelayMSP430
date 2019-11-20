#!/usr/bin/env bash
if [ -n "$1" ] # Если запустили без параметров
#  if [ "$EUID" -ne 0 ] <-- проверка запуска от рута
  then
  	# For working with MSP430F5529LP using mspdebug interface,
  	# copy libmsp430.so to '/usr/lib/' folder 
	cp libmsp430.so /usr/lib/libmsp430.so
	echo "Библиотека libmsp430 установлена"
  else # А если с параметрами
  	rm -f balloon.bin
  	if [ "$(whoami)" != "osabio" ]
  	then
		echo "Запуск msp430-gcc"
  		msp430-gcc  -Os -mmcu=msp430f5529 -o balloon.bin balloon.c
  	else
		echo "Запуск msp430-elf-gcc"
		msp430-elf-gcc -I /opt/ti/msp430-gcc/include -Os -mmcu=msp430f5529 -o balloon.bin balloon.c
  	fi
	mspdebug tilib "prog balloon.bin"
fi

