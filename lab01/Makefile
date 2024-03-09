CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) -o countdown countdown.c

clean:
	rm -f countdown