#include "../include/io.h"
#include <stdio.h>
#include <stdlib.h>

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

static void fill_array_null(char *arr, int len)
{
	int i;
	for(i = 0; i < len; i++)
		arr[i] = 0;
}

static char *read_word(char **ptr_str, char *w, int max_size)
{
	char new_word[max_size];
	int flag_quote = 0;
	int flag_escape_char = 0;
	char *tmp1, *tmp2;
	int counter = 0;

	fill_array_null(new_word, max_size);
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
		*ptr_str = malloc(sizeof(char) * counter);
		copy_str(new_word, *ptr_str, counter);
		(*ptr_str)[counter] = '\0';
	} else {
		*ptr_str = NULL;
	}

	return tmp1;
}

static int count_words(char *str)
{
	char *tmp = str;
	int counter = 0;

	while(!is_end_of_string((const char *)tmp)) {
		if(is_space_char((const char *)tmp))
			counter++;
		tmp++;
	}
	
	counter++;
	return counter;
}

char **split_string(struct string_item *si)
{
	char **ptr_str = NULL, **tmp_str;
	char *tmp = NULL;
	int counter;

	tmp = si->str;

	while(tmp <= tmp + si->length - 1) {
		if(check_empty_word(tmp))
			tmp = tmp + 2;

		if(*tmp == ' ' || *tmp == '\t') {
			tmp++;
			continue;
		}

		if(ptr_str == NULL) {
			counter = count_words(si->str);
			ptr_str = malloc(counter * sizeof(char **) + 1);
			tmp_str = ptr_str;
		}
		else {
			ptr_str++;
		}

		tmp = read_word(ptr_str, tmp, si->max_size);
		if(*ptr_str == NULL)
			break;
	}

	return tmp_str;
}

void release_string_item(struct string_item *si)
{
	free(si->str);
	free(si);
}
