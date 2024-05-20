CC = gcc
CFLAGS = -Wall

all: life
	
life: grid.c life.c
	$(CC) $(CFLAGS) grid.c life.c -o life -lncursesw -L/usr/local/opt/ncurses/lib
	
clean:
	rm -f life
