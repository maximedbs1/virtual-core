CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

all: main

main: main.o
	$(CC) $(CFLAGS) -o $@ $<

main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f main main.o