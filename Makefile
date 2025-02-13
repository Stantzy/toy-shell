ifeq ($(origin CC),default)
	CC = gcc
endif

CFLAGS ?= -Wall -g
OUT_O_DIR ?= build
INPUT_SRC = ./src/input/input_reader.c
TOKENIZER_SRC = ./src/tokenizer/analyze_token.c ./src/tokenizer/check_errors.c ./src/tokenizer/tokenizer.c
EXEC_SRC = ./src/exec/exec_options.c ./src/exec/executor.c ./src/exec/handlers.c ./src/exec/pipeline.c
ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

CSRC = ./src/main.c $(INPUT_SRC) $(TOKENIZER_SRC) $(EXEC_SRC)

COBJ := $(addprefix $(OUT_O_DIR)/,$(CSRC:.c=.o))
DEPS := $(COBJ:.o=.d)

%.o : ./src/%/%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: ./src/%/%.c
	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

include $(DEPS)

.PHONY: all clean
all: $(OUT_O_DIR)/main.x

$(OUT_O_DIR)/main.x: $(COBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(COBJ) : $(OUT_O_DIR)/%.o : %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEPS) : $(OUT_O_DIR)/%.d : %.c
	@mkdir -p $(@D)
	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

clean:
	@echo "Removing everything but the source files"
	@rm -rf $(COBJ) $(DEPS) $(OUT_O_DIR)/*.x

NODEPS = clean

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	include $(DEPS)
endif
