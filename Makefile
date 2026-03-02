CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c23 -Iinclude -O3 -g -fno-omit-frame-pointer

LINKFLAGS = -flto -static
LDLIBS = -lm

TESTLINKFLAGS =
TESTLDLIBS = -lcmocka

.PHONY: all clean run test

all: bin/solution_naive_main bin/solution_vector_main bin/solution_hashmap_main

clean:
	rm -rf build bin perf.data perf.data.old out.perf out.folded flamegraph.svg

run: all
	bin/solution_hashmap_main data/measurements_100M.csv /dev/null

test: bin/test_vector bin/test_fast_strtof
	bin/test_vector
	bin/test_fast_strtof

perf.data: bin/solution_hashmap_main
	perf record -F 1000 -g --debuginfod --call-graph dwarf bin/solution_hashmap_main data/measurements_100M.csv /dev/null


flamegraph.svg: perf.data
	perf script > out.perf
	stackcollapse-perf.pl out.perf > out.folded
	flamegraph.pl out.folded > flamegraph.svg


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
bin/solution_hashmap_main: build/solution_hashmap.o build/solution_hashmap_main.o build/fast_strtof.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LINKFLAGS)

build/solution_hashmap.o: src/solution_hashmap.c include/solution_hashmap.h include/solution_vector.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_hashmap.c -o $@

build/solution_hashmap_main.o: src/solution_hashmap_main.c
	mkdir -p build
	$(CC) $(CFLAGS) -c src/solution_hashmap_main.c -o $@


# Library fast_strtof
build/fast_strtof.o: src/fast_strtof.c include/fast_strtof.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/fast_strtof.c -o $@


# Tests
bin/test_vector: tests/test_vector.c include/vector_generic.h
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(TESTLDLIBS) $(TESTLINKFLAGS)

bin/test_fast_strtof: tests/test_fast_strtof.c build/fast_strtof.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(TESTLDLIBS) $(TESTLINKFLAGS)
