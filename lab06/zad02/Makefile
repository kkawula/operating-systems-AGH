CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: main integral

main: main.c
	$(CC) $(CFLAGS) -o main main.c

integral: integral.c
	$(CC) $(CFLAGS) -o integral integral.c

clean:
	rm -rf main integral pipe

run: main integral
	./main
	./integral


