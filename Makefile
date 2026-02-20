CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c23 -Iinclude
LDLIBS = -lm

.PHONY: all clean

all: bin/naive_main bin/naive_with_vector_main

clean:
	rm -rf build bin


# Naive approach
bin/naive_main: build/naive.o build/naive_main.o
	mkdir -p bin
	$(CC) $^ $(LDLIBS) -o $@

build/naive.o: src/naive.c include/naive.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive.c -o $@

build/naive_main.o: src/naive_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive_main.c -o $@


# Naive approach with a vector
bin/naive_with_vector_main: build/naive_with_vector.o build/naive_with_vector_main.o
	mkdir -p bin
	$(CC) $^ $(LDLIBS) -o $@

build/naive_with_vector.o: src/naive_with_vector.c include/naive_with_vector.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive.c -o $@

build/naive_with_vector_main.o: src/naive_with_vector_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/naive_main.c -o $@
