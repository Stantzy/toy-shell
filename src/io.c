#include <stdio.h>
#include <stdlib.h>
#include "../include/io.h"

static int is_space_char(const char *c)
{
	return *c == ' ' || *c == '\t';
}

static int is_end_of_string(const char *c)
{
	return *c == '\n' || *c == '\0';
}

static int is_to_read_word(const char *c, int flag)
{
	return (!is_space_char(c) || flag) && (!is_end_of_string(c));
}

static int check_empty_word(const char *tmp)
{
	return *tmp == '"' && *(tmp + 1) == '"';
}

int check_only_space_chars(const char *str)
{
	int flag = 1;
	
	while(*str != '\0') {
		if(*str != ' ' && *str != '\t')
			flag = 0;
		str++;
	}

	return flag;
}

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

	if(counter % 2 != 0)
		return 1;

	return 0;
}

static void copy_str(const char *src, char *dst, int length)
{
	int i;
	
	for(i = 0; i < length; i++)
		dst[i] = src[i];
}


static void resize_array(struct string_item *si)
{
	char *new_arr;

	new_arr = malloc(sizeof(char) * si->max_size * 2);
    copy_str(si->str, new_arr, si->length);
	free(si->str);

	si->str = new_arr;
	si->max_size = si->max_size * 2;
}

int read_string(struct string_item *si)
{
	char c;
	char *tmp;
	int counter;

	counter = 0;
	tmp = si->str;
	si->length = 0;

	while((c = getchar()) != '\n') {
		if(c == EOF)
			return 1;

		*tmp = c;
		tmp++;
		counter++;
		si->length = counter;

		if(tmp > si->str + si->max_size) {
			resize_array(si);
			tmp = si->str + si->length;
		}
	}
	
	*tmp = '\0';

	return 0;
}

static char *read_word(struct word_item *wi, char *w, int max_size)
{
	enum { buff = 128 };
	char new_word[max_size];
	int flag_quote = 0;
	int flag_escape_char = 0;
	char *tmp1, *tmp2;
	int counter = 0;

	tmp1 = w;
	tmp2 = new_word;
 	while(is_to_read_word(tmp1, flag_quote)) {
		if(*tmp1 == '\\' && !flag_escape_char) {
			flag_escape_char = 1;
			tmp1++;
			continue;
		}

		if(flag_quote && *tmp1 == '"' && !flag_escape_char)
			flag_quote = 0;
		else
		if(!flag_quote && *tmp1 == '"' && !flag_escape_char)
			flag_quote = 1;	

		if(*tmp1 == '"' && !flag_escape_char) {
			tmp1++;
			continue;
		}

		flag_escape_char = 0;
		counter++;
		*tmp2 = *tmp1;
		tmp1++;
		tmp2++;
	}
	
	if(counter != 0) {
		wi->word = malloc(sizeof(char) * counter + 1);
		copy_str(new_word, wi->word, counter);
		(wi->word)[counter] = '\0';
	} else {
		wi->word = NULL;
	}

	return tmp1;
}

struct word_item *split_string(struct word_item *wi, struct string_item *si)
{
	struct word_item *new_wi = NULL, *tmp_wi = NULL;
	char *tmp;

	tmp = si->str;
	while(tmp <= tmp + si->length - 1) {
		if(check_empty_word(tmp))
			tmp = tmp + 2;

		if(*tmp == ' ' || *tmp == '\t') {
			tmp++;
			continue;
		}

		if(new_wi == NULL) {
			new_wi = malloc(sizeof(struct word_item));
			tmp_wi = new_wi;
		}
		else {
			tmp_wi->next = malloc(sizeof(struct word_item));
			tmp_wi = tmp_wi->next;
		}

		tmp = read_word(tmp_wi, tmp, si->max_size);
		if(tmp_wi->word == NULL)
			break;
	}
	
	tmp_wi->next = NULL;

	return new_wi;
}

void output_words(struct word_item *wi)
{
    struct word_item *tmp;
    
    tmp = wi;
	while(tmp->word != NULL) {
		printf("[%s]", tmp->word);
		tmp = tmp->next;
	}
}

void release_string_item(struct string_item *si)
{
	free(si->str);
	free(si);
}

void release_word_item(struct word_item *wi)
{
	struct word_item *tmp = wi;
	
	while(tmp) {
		wi = tmp->next;
		free(tmp->word);
		free(tmp);
		tmp = wi;
	}
}
