CC=gcc
CFLAGS=-c -Wall -g
input_sources="./src/input/input_reader.c"
tokenizer_sources="./src/tokenizer/analyze_token.c" "./src/tokenizer/check_errors.c" "./src/tokenizer/tokenizer.c"
exec_sources="./src/exec/exec_options.c" "./src/exec/executor.c" "./src/exec/handlers.c"

all: final

final: main.o exec.o input.o tokenizer.o
	@echo "Compiling complete. Linking and producing the final application..."
	@$(CC) -Wall -g *.o

main.o:
	@echo "Compiling the main file..."
	@$(CC) $(CFLAGS) ./src/main.c

exec.o:
	@echo "Compiling executor files..."
	@$(CC) $(CFLAGS) $(exec_sources)

input.o:
	@echo "Compiling input reader files..."
	@$(CC) $(CFLAGS) $(input_sources)

tokenizer.o:
	@echo "Compiling tokenizer files..."
	@$(CC) $(CFLAGS) $(tokenizer_sources)

clean:
	@echo "Removing everything but the source files"
	@rm *.o a.out