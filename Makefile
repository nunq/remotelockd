CC=gcc
CFLAGS=-pipe -pedantic-errors -Wall -Wextra -Wcast-align -O2
LDFLAGS=$(pkg-config --cflags --libs libnotify libsystemd)
BIN=rlockd
SOURCE=rlockd.c
all: main
	
main: $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN) $(SOURCE)

clean: $(BIN)
	rm $(BIN)
