#include "../../include/tokens.h"
#include "../../include/tokenizer/analyze_token.h"

#include <stdio.h>
#include <stdlib.h>

static int is_separator_double(const char *c)
{
    return *c == *(c + 1);
}

char *handle_separator(const char *str, int *offset)
{
    char *word, *word_start;
    int flag_double = 0;

    word = malloc(sizeof(char) * word_size);
    word_start = word;
    

    switch(*str) {
        case '>':
            flag_double = is_separator_double(str);
            break;
        case '&':
        case '<':
            break;
        case '|':
        case ';':
        case '(':
        case ')':
            fprintf(stderr, "Feature '%c' not implemented yet\n", *str);
        default:
            free(word);
            (*offset)++;
            return NULL;
    }

    *word = *str;
    if(flag_double) {
        /* if this is a double case, 
        we need to move our pointer two positions further */
        *(word + 1) = *(str + 1);
        *(word + 2) = '\0';
        *offset += 2;
    } else {
        *(word + 1) = '\0';
        (*offset)++;
    }

    return word_start;
}

static int is_end_of_word(char c)
{
    return c == '\0' || c == ' ' || c == '\t';
}

int is_separator(char c)
{
    return c == '&' || c == '>' || c == '<' ||
            c == ';' || c == '(' || c == ')' || c == '|';
}

static void handle_flag(int *flag, const char **str_ptr, int *off)
{
    *flag = -1 * (*flag);
    (*str_ptr)++;
    (*off)++;
}

char *read_word(const char *string, int *offset)
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
        if(is_separator(*string) &&
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