CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c23 -Iinclude
LDLIBS = -lm -lcmocka

.PHONY: all clean run test

all: bin/naive_main bin/naive_with_vector_main

clean:
	rm -rf build bin

run: all
	bin/naive_with_vector_main data/measurements_1m.csv >/dev/null

test: bin/test_vector
	bin/test_vector

# Naive approach
bin/naive_main: build/naive.o build/naive_main.o
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $@

build/naive.o: src/naive.c include/naive.h
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) -c src/naive.c -o $@

build/naive_main.o: src/naive_main.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) -c src/naive_main.c -o $@


# Naive approach with a vector
bin/naive_with_vector_main: build/naive_with_vector.o build/naive_with_vector_main.o
	mkdir -p bin
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $@

build/naive_with_vector.o: src/naive_with_vector.c include/naive_with_vector.h
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) -c src/naive.c -o $@

build/naive_with_vector_main.o: src/naive_with_vector_main.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) -c src/naive_main.c -o $@


# Vector lib
build/vector.o: lib/vector.c include/vector.h
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) -c lib/vector.c -o $@

bin/test_vector: tests/test_vector.c build/vector.o
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $@
