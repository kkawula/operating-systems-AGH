CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c
clean:
	rm -rf main

run: main
	./main none
	./main ignore
	./main mask

