CC=gcc
CFLAGS=-c -Wall -g

all: main.o io.o run.o
	$(CC) ./src/main.o ./src/io.o ./src/run.o -o main

main.o: ./src/main.c
	$(CC) $(CFLAGS) ./src/main.c -o ./src/main.o

io.o: ./src/io.c
	$(CC) $(CFLAGS) ./src/io.c -o ./src/io.o

run.o: ./src/run.c
	$(CC) $(CFLAGS) ./src/run.c -o ./src/run.o
clean:
	rm ./src/*.o
