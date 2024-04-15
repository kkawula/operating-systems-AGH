CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: sender catcher

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender

catcher: catcher.c
	$(CC) $(CFLAGS) catcher.c -o catcher
	
clean:
	rm -rf sender catcher


