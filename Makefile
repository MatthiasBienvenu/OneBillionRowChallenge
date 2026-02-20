CC = gcc
CFLAGS = -Wall -Wextra -std=c23 -Iinclude
LDLIBS = -lcmocka -lm

TARGET = bin/test_1b_challenge

OBJS = build/1b_challenge.o build/1b_challenge_main.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(OBJS) $(LDLIBS) -o $(TARGET)

build/1b_challenge.o: src/1b_challenge.c include/1b_challenge.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/1b_challenge.c -o build/1b_challenge.o

build/1b_challenge_main.o: src/1b_challenge_main.c include/1b_challenge.h
	mkdir -p build
	$(CC) $(CFLAGS) -c src/1b_challenge_main.c -o build/1b_challenge_main.o

clean:
	rm -rf build bin
