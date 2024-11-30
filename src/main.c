#include <stdio.h>
#include <stdlib.h>
#include "../include/io.h"

int main()
{
	struct string_item *si;
	struct word_item *wi;
	int result_flag = 0;

 	si = malloc(sizeof(struct string_item));
	si->str = malloc(sizeof(char) * start_buffer);
	si->max_size = start_buffer;
	
	while(result_flag == 0) {
		printf("> ");
		result_flag = read_string(si);
		
		if(result_flag == 1) {
			putchar('\n');
			break;
		}

		if(si->length == 0)
			continue;

		if(check_only_space_chars(si->str))
			continue;

		if(check_quotes(si->str)) {
			printf("Error: unmatched quotes\n");
			continue;
		}
		
		wi = split_string(wi, si);
		output_words(wi);
		release_word_item(wi);

		putchar('\n');
	}

	release_string_item(si);

	return 0;
}
