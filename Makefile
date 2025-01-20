CC=gcc
CFLAGS=-c -Wall -g

all: final

final: main.o input_reader.o tokenizer.o executor.o
	@echo "Compiling complete. Linking and producing the final application..."
	$(CC) ./src/main.o ./src/input_reader.o ./src/tokenizer.o ./src/executor.o -o main

main.o: ./src/main.c
	@echo "Compiling the main file..."
	@$(CC) $(CFLAGS) ./src/main.c -o ./src/main.o

input_reader.o: ./src/input_reader.c
	@echo "Compiling the input reader file..."
	@$(CC) $(CFLAGS) ./src/input_reader.c -o ./src/input_reader.o

tokenizer.o: ./src/tokenizer.c
	@echo "Compiling the tokenizer file..."
	@$(CC) $(CFLAGS) ./src/tokenizer.c -o ./src/tokenizer.o

executor.o: ./src/executor.c
	@echo "Compiling the executor file..."
	@$(CC) $(CFLAGS) ./src/executor.c -o ./src/executor.o

clean:
	@echo "Removing everything but the source files"
	@rm ./src/*.o main
