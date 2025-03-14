#include "../include/tokens.h"
#include "../include/input/input_reader.h"
#include "../include/tokenizer/tokenizer.h"
#include "../include/exec/executor.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *str_ptr;
	struct token_item *cmd, *tmp;

	while((str_ptr = get_input()) != NULL) {
		cmd = tokenize_string(str_ptr);
		tmp = cmd;

		if(cmd != NULL) 
			exec_prog(cmd);

		release_memory(tmp);
		free(str_ptr);
	}

	return 0;
}
