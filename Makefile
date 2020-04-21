CC=gcc
CFLAGS=-std=gnu99 -pipe -pedantic-errors -Wall -Wextra -Wcast-align -O2
LD_CFLAGS=$(shell pkg-config --cflags libnotify libsystemd)
LDFLAGS=$(shell pkg-config --libs libnotify libsystemd)
BIN=rlockd
SOURCE=rlockd.c
all: main
	
main: $(SOURCE)
	$(CC) $(CFLAGS) $(LD_CFLAGS) $(LDFLAGS) -o $(BIN) $(SOURCE)

clean: $(BIN)
	rm $(BIN)
