CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c23 -Iinclude
LDLIBS = -lm

.PHONY: all clean

all: bin/naive_main

bin/naive_main: build/naive.o build/naive_main.o
	mkdir -p bin
	$(CC) $^ $(LDLIBS) -o $@

build/naive.o: src/naive.c include/naive.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive.c -o $@

build/naive_main.o: src/naive_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive_main.c -o $@

clean:
	rm -rf build bin
