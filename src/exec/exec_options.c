#include "../shared_include/tokens.h"
#include "exec_structs.h"
#include "exec_options.h"

#include <stdlib.h>
#include <string.h>

void init_options(struct exec_options *o)
{
    o->background = 0;
    o->redirection_in = 0;
    o->redirection_out = 0;
    o->redirection_out_rewrite = 0;
    o->in_path = NULL;
    o->out_path = NULL;
}

static int is_out_path(struct exec_options o, int token_type)
{
    return token_type == regular_token &&
            (o.redirection_out == 1 || o.redirection_out_rewrite == 1);
}

static int is_in_path(struct exec_options o, int token_type)
{
    return token_type == regular_token && o.redirection_in == 1;
}

static int is_redirection_separator(struct token_item *token)
{
    if(token->type == separator)
        return *(token->word) == '<' || *(token->word) == '>';
    return 0;
}

int update_options(struct token_item *first, struct exec_options *o)
{
    int rdir_out_flag = 0;

    while(first != NULL) {
        if(rdir_out_flag && is_redirection_separator(first)) {
            init_options(o);
            return 1;
        }

        if(strcmp(first->word, "&") == 0 && first->type == separator) {
            o->background = 1;
        } else
        if(strcmp(first->word, ">") == 0 && first->type == separator) {
            o->redirection_out_rewrite = 1;
            rdir_out_flag = 1;
        } else 
        if(strcmp(first->word, ">>") == 0 && first->type == separator) {
            o->redirection_out = 1;
            rdir_out_flag = 1;
        } else
        if(strcmp(first->word, "<") == 0 && first->type == separator) {
            o->redirection_in = 1;
        }

        if(is_out_path(*o, first->type))
            o->out_path = first->word;
        else
        if(is_in_path(*o, first->type))
            o->in_path = first->word;

        first = first->next;
    }

    return 0;
}