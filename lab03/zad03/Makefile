CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: flipper run

flipper: flipper.c
	$(CC) $(CFLAGS) -o flipper flipper.c

clean:
	rm -rf flipper out

run:
	./flipper . ./out

