CFLAGS=-Wall -Werror
LDFLAGS=

BIN=1b_challenge

all: $(BIN)


1b_challenge: src/1b_challenge.c src/1b_challenge_main.c src/1b_challenge.h
	mkdir -p build
	$(CC) -o build/1b_challenge src/1b_challenge.c src/1b_challenge_main.c $(CFLAGS) $(LDFLAGS)

data: scripts/generate_data.py
	python scripts/generate_data.py

test: test_1b_challenge
	build/test_1b_challenge

test_1b_challenge: src/test_1b_challenge.c src/1b_challenge.c src/1b_challenge.h
	mkdir -p build
	$(CC) -o build/test_1b_challenge src/test_1b_challenge.c src/1b_challenge.c -lcmocka $(CFLAGS) $(LDFLAGS)

clean:
	rm -r build
