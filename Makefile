CC=gcc
CFLAGS=-std=c99 -pipe -pedantic-errors -Wall -Werror -Wextra -Wcast-align -O2
LDFLAGS=-lsystemd
BIN=rlockd
SOURCE=rlock.c
all: main
	
main: rlockd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN) $(SOURCE)

clean: hue
	rm $(BIN)
