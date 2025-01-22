#include "../shared_include/tokens.h"
#include "check_errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_quotes(const char *str)
{
	int counter = 0;
	int flag_escape_char = 0;
	
	while(*str != '\0') {
		if(*str == '\\' && !flag_escape_char) {
			flag_escape_char = 1;
			str++;
			continue;
		}

		if(*str == '"' && !flag_escape_char)
			counter++;

		flag_escape_char = 0;
		str++;
	}

	return counter % 2;
}

int check_separators(struct token_item *first)
{
    int rdir_out_flag = 0;
    int rdir_in_flag = 0;
    int cur_flag = 0;

    while(first != NULL) {
        cur_flag = 0;
        if(rdir_in_flag && first->type == separator && *(first->word) == '<')
            return 1;
        if(rdir_out_flag && first->type == separator && *(first->word) == '>')
            return 1;

        if(strcmp(first->word, ">") == 0 && first->type == separator) {
            rdir_out_flag = 1;
            cur_flag = 1;
        } else 
        if(strcmp(first->word, ">>") == 0 && first->type == separator) {
            rdir_out_flag = 1;
            cur_flag = 1;
        } else
        if(strcmp(first->word, "<") == 0 && first->type == separator) {
            rdir_in_flag = 1;
            cur_flag = 1;
        }

        first = first->next;
        if(cur_flag && first != NULL && first->type == separator)
            return 2;
    }

    return 0;
}