CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown

clean:
	rm -rf countdown countdown.dSYM