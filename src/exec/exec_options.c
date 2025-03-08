#include "../../include/tokens.h"
#include "../../include/exec/exec_structs.h"
#include "../../include/exec/exec_options.h"

#include <stdio.h>
#include <string.h>

void init_options(struct exec_options *o)
{
	o->background = 0;
	o->count_pipelines = 0;
	o->cmd_pgid = 0;
	o->rdir_in_flag = 0;
	o->rdir_out_flag = 0;
	o->rdir_append_flag = 0;
	o->save_stdin = 0;
	o->save_stdout = 1;
	o->cur_fdin = 0;
	o->cur_fdout = 1;
	o->in_path = NULL;
	o->out_path = NULL;
}

static int is_path(int flag, int token_type)
{
	return token_type == regular_token && flag == 1;
}

static int is_redirection_separator(struct token_item *token)
{
	if(token->type == separator)
		return *(token->word) == '<' || *(token->word) == '>';
	return 0;
}

int update_options(struct token_item *first, struct exec_options *o)
{
	int rdir_out_flag = 0, rdir_in_flag = 0;

	while(first != NULL) {
		if(rdir_out_flag && is_redirection_separator(first)) {
			init_options(o);
			return 1;
		}

		if((rdir_out_flag == 1 || rdir_in_flag == 1) &&
		first->type == separator) {
			fprintf(stderr, "Specify the file name to redirect\n");
			return -1;
		}

		if(strcmp(first->word, "|") == 0 && first->type == separator) {
			(o->count_pipelines)++;
		} else
		if(strcmp(first->word, "&") == 0 && first->type == separator) {
			o->background = 1;
		} else
		if(strcmp(first->word, ">") == 0 && first->type == separator) {
			o->rdir_out_flag = 1;
			rdir_out_flag = 1;
		} else 
		if(strcmp(first->word, ">>") == 0 && first->type == separator) {
			o->rdir_append_flag = 1;
			rdir_out_flag = 1;
		} else
		if(strcmp(first->word, "<") == 0 && first->type == separator) {
			o->rdir_in_flag = 1;
			rdir_in_flag = 1;
		}

		if(is_path(rdir_out_flag, first->type)) {
			o->out_path = first->word;
			rdir_out_flag = 0;
		} else 
		if(is_path(rdir_in_flag, first->type)) {
			o->in_path = first->word;
			rdir_in_flag = 0;
		}

		first = first->next;
	}

	return 0;
}
