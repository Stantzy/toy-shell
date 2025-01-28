#include "../../include/tokens.h"
#include "../../include/tokenizer/check_errors.h"
#include "../../include/tokenizer/analyze_token.h"
#include "../../include/tokenizer/tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct token_item *init_item()
{
	struct token_item *new_item = NULL;
	
	new_item = malloc(sizeof(struct token_item));
	new_item->word = NULL;
	new_item->next = NULL;

	return new_item;
}

void release_memory(struct token_item *first)
{
	struct token_item *tmp;

	while(first != NULL) {
		tmp = first->next;
        free(first->word);
		free(first);
		first = tmp;
	}
}

static void update_token_info(struct token_item *tkn, char *s, int t)
{
    tkn->word = s;
    tkn->type = t;
}

static int handle_sep_check(int res)
{
    if(res == 1)
        fprintf(stderr, "Error: double I/O stream redirection detected\n");
    else
    if(res == 2) 
        fprintf(stderr, "Error: unexpected token\n");

    if(res != 0)
        return 1;
    return 0;
}

struct token_item *tokenize_string(const char *str)
{
	struct token_item *first = NULL, *tmp = NULL;
	const char *str_start = str;
	char *token = NULL;
    int offset = 0, token_type = 0, res;
	
	if(str == NULL) {
		return NULL;
	}

    if(check_quotes(str) != 0) {
        fprintf(stderr, "Error: unmatched quotes\n");
        return NULL;
	}

	while(*str != '\0') {
        if(token_type == separator && token != NULL &&
        strcmp(token, "&") == 0 && *str != ' ' && *str != '\t') {
            fprintf(
                stderr,
                "Error: non-whitespace characters found after '&'\n"
                );
            release_memory(first);
            return NULL;
        }
        if(is_separator(*str)) {
            token = handle_separator(str, &offset);
            token_type = separator;
        } else {
            token = read_word(str, &offset);
            token_type = regular_token;
        }
        
        if(token == NULL) {
            str = str_start + offset;
            continue;
        }

        if(first == NULL) {
            first = init_item();
            tmp = first;
        } else {
            tmp->next = init_item();
            tmp = tmp->next;
        }
        
        update_token_info(tmp, token, token_type);
        str = str_start + offset;
	}

    res = check_separators(first);
    if(handle_sep_check(res)) {
        release_memory(first);
        return NULL;
    }

	return first;
}

