#include "input_reader.h"

#include <stdio.h>
#include <stdlib.h>

static void welcome()
{
	printf("> ");
}

static void copy_str(const char *src, char *dst, int length)
{
	int i;
	for(i = 0; i < length; i++)
		dst[i] = src[i];
}

static void resize_array(char **arr, int *old_size)
{
	char *new_arr;
	
	new_arr = malloc(sizeof(char) * (*old_size) * 2);
    copy_str(*arr, new_arr, *old_size);
	free(*arr);
	*old_size *= 2;
	*arr = new_arr;
}

char *get_input()
{
	char *new_string, *tmp;
	char c;
	int buffer = start_buffer;
	int counter = 0;

	new_string = malloc(sizeof(char) * buffer);	
	tmp = new_string;

	welcome();

	while((c = getchar()) != '\n') {
		if(c == EOF) {
			free(new_string);
			return NULL;
		}

		*tmp = c;
		tmp++;
		counter++;

		if(counter >= buffer) {
			resize_array(&new_string, &buffer);
			tmp = new_string + counter;
		}
	}

	*tmp = '\0';

	return new_string;
}