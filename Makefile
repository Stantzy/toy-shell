CC=gcc
CFLAGS=-c -Wall -g

all: main.o io.o 
	$(CC) ./src/main.o ./src/io.o -o main

main.o: ./src/main.c
	$(CC) $(CFLAGS) ./src/main.c -o ./src/main.o

io.o: ./src/io.c
	$(CC) $(CFLAGS) ./src/io.c -o ./src/io.o

clean:
	rm -rf ./src/*.o
