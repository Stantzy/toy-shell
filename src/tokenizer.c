#include "../include/tokens.h"
#include "../include/tokenizer.h"

#include <stdio.h>
#include <stdlib.h>

static int check_quotes(const char *str)
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

static int is_end_of_word(char c)
{
    return c == '\0' || c == ' ' || c == '\t';
}

static void handle_flag(int *flag, const char **str_ptr, int *off)
{
    *flag = -1 * (*flag);
    (*str_ptr)++;
    (*off)++;
}

static char *handle_special_token(const char *str, int *offset)
{
    char *word, *word_start;

    word = malloc(sizeof(char) * word_size);
    word_start = word;
    (*offset)++;

    switch(*str) {
        case '&':
            //check_double_case(); for "&&" 
            *word = *str;
            *(word + 1) = '\0';
            return word_start;
        case '>':
            //check_double_case(); for ">>"
        case '<':
        case '|':
            //check_double_case(); for "||"
        case ';':
        case '(':
        case ')':
            fprintf(stderr, "Feature '%c' not implemented yet\n", *str);
        //case ">>":
        //case "&&":
        //case "||":
        default:
            free(word);
            return NULL;
    }
}

static void update_token_info(struct token_item *tkn, char *s, int t)
{
    tkn->word = s;
    tkn->type = t;
}

static int is_special_token(char c)
{
    return c == '&' || c == '>' || c == '<' ||
            c == ';' || c == '(' || c == ')';
}

static char *read_word(const char *string, int *offset)
{
    char *word, *word_start;
    int quotes_flag = -1;   /* -1 = outside quotes, 1 = inside quotes */
    int escape_char_flag = -1;  /* -1 = not set, 1 = set */

    word = malloc(sizeof(char) * word_size);
    word_start = word;

    if(*string == ' ' || *string == '\t') {
        (*offset)++;
        free(word);
        return NULL;
    }

    while(!is_end_of_word(*string) || quotes_flag == 1) {
        if(is_special_token(*string) &&
                quotes_flag == -1 &&
                escape_char_flag == -1)
            break;
        
		if(*string == '\\' && quotes_flag == -1 && escape_char_flag == -1) {
			handle_flag(&escape_char_flag, &string, offset);
			continue;
		}
        if(*string == '"' && escape_char_flag == -1) {
            handle_flag(&quotes_flag, &string, offset);
			continue;
        }

        *word = *string;
        string++;
        word++;
        (*offset)++;
		escape_char_flag = -1;
    }

	*word = '\0';

    return word_start;
}

struct token_item *tokenize_string(const char *str)
{
	struct token_item *first = NULL, *tmp = NULL;
	const char *str_start = str;
	char *token = NULL;
    int offset = 0;
    int token_type = 0;
	
	if(str == NULL) {
		return NULL;
	}

    if(check_quotes(str) != 0) {
        fprintf(stderr, "Error: unmatched quotes\n");
        return NULL;
	}

	while(*str != '\0') {
        if(token_type == separator && *str != ' ' && *str != '\t') {
            fprintf(
                stderr,
                "Error: non-whitespace characters found after '&'\n"
                );
            release_memory(first);
            return NULL;
        }
        if(is_special_token(*str)) {
            token = handle_special_token(str, &offset);
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

	return first;
}

