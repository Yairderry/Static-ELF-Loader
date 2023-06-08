all: loader

loader: loader.o startup.o start.o
	ld -o loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

loader.o: loader.c
	gcc -m32 -c loader.c -o loader.o 

start.o: start.s
	nasm -g -f elf32 -w+all -o start.o start.s

startup.o: startup.s
	nasm -g -f elf32 -w+all -o startup.o startup.s 

.PHONY: clean

clean:
	rm -f *.o loader
