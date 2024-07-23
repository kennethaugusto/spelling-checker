CC=gcc
CFLAGS= -Wall -Werror -std=gnu99 -g -O0

all: spchk

spchk:spchk.c empty.c file_handler.c file_handler.h dict_handler.c dict_handler.h processFile.c processFile.h
	$(CC) $(CFLAGS) -o $@ $(wildcard *.c)  

clean:
	rm -f spchk