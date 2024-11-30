#ifndef IO_H_SENTRY
#define IO_H_SENTRY

enum { start_buffer = 128 };

struct string_item {
	char *str;
	int length;
	int max_size;
};

struct word_item {
	char *word;
	struct word_item *next;
};

int check_only_space_chars(const char *str);
int check_quotes(const char *str);
int read_string(struct string_item *si);
struct word_item *split_string(struct word_item *wi, struct string_item *si);
void output_words(struct word_item *wi);
void release_string_item(struct string_item *si);
void release_word_item(struct word_item *wi);

#endif
