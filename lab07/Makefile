CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: client server

client0: client.c
	$(CC) $(CFLAGS) -o client0 client.c

client1: client.c
	$(CC) $(CFLAGS) -o client1 client.c

server: server.c
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -rf client0 client1 server




