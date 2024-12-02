#ifndef IO_H_SENTRY
#define IO_H_SENTRY

enum { start_buffer = 128 };

struct string_item {
	char *str;
	int length;
	int max_size;
};

int check_only_space_chars(const char *str);
int check_quotes(const char *str);
int read_string(struct string_item *si);
char **split_string(struct string_item *si);
void output_words(char **str);
void release_string_item(struct string_item *si);

#endif
