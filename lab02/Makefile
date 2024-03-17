CC = gcc
CFLAGS = -Wall -g -O0

.PHONY: all clean test

all: static_client shared_client dynamic_client

static_client: client.c static_lib
	$(CC) $(CFLAGS) -o static_client client.c libcollatz.a

shared_client: client.c shared_lib
	$(CC) $(CFLAGS) -o shared_client client.c -lcollatz -L ./ -Wl,-rpath,.

dynamic_client: client.c shared_lib
	$(CC) $(CFLAGS) -o dynamic_client client.c -D DYNAMIC_LIB

static_lib: collatz.c
	$(CC) $(CFLAGS) -c collatz.c
	ar rcs libcollatz.a collatz.o

shared_lib: collatz.c
	$(CC) $(CFLAGS) -fPIC -c collatz.c
	$(CC) $(CFLAGS) -shared -o libcollatz.so collatz.o

collatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -c collatz.c

test: static_client shared_client dynamic_client
	./static_client
	./shared_client
	./dynamic_client

clean:
	rm -rf static_client shared_client dynamic_client libcollatz.a libcollatz.so collatz.o dynamic_client.dSYM shared_client.dSYM static_client.dSYM
