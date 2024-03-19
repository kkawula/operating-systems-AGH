CC = gcc
CFLAGS = -Wall -g

.PHONY: all clean
all: finder run

finder: finder.c
	$(CC) $(CFLAGS) -o finder finder.c
clean:
	rm -rf example.txt finder finder.dSYM

run: example.txt
	./finder example.txt

example.txt:
	 echo "Hello world!" > example.txt
