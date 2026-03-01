CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c23 -Iinclude -O3 -g -fno-omit-frame-pointer

LINKFLAGS = -flto -static
LDLIBS = -lm

TESTLINKFLAGS =
TESTLDLIBS = -lcmocka

.PHONY: all clean run test perf

all: bin/solution_naive_main bin/solution_vector_main bin/solution_hashmap_main

clean:
	rm -rf build bin

run: all
	bin/solution_hashmap_main data/measurements_10m.csv /dev/null

test: bin/test_vector
	bin/test_vector

perf: bin/solution_hashmap_main
	perf record --debuginfod --call-graph dwarf make run
	perf report

# Naive approach
bin/solution_naive_main: build/solution_naive.o build/solution_naive_main.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LINKFLAGS)

build/solution_naive.o: src/solution_naive.c include/solution_naive.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_naive.c -o $@

build/solution_naive_main.o: src/solution_naive_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_naive_main.c -o $@


# Naive approach with a vector
bin/solution_vector_main: build/solution_vector.o build/solution_vector_main.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LINKFLAGS)

build/solution_vector.o: src/solution_vector.c include/solution_vector.h include/solution_vector.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_vector.c -o $@

build/solution_vector_main.o: src/solution_vector_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_vector_main.c -o $@


# Approach with a hashmap
bin/solution_hashmap_main: build/solution_hashmap.o build/solution_hashmap_main.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LINKFLAGS)

build/solution_hashmap.o: src/solution_hashmap.c include/solution_hashmap.h include/solution_vector.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_hashmap.c -o $@

build/solution_hashmap_main.o: src/solution_hashmap_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_hashmap_main.c -o $@


# Vector lib
bin/test_vector: tests/test_vector.c include/vector_generic.h
	# no link flags
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(TESTLDLIBS) $(TESTLINKFLAGS)
