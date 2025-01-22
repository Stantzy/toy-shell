#ifndef TOKENS_H_SENTRY
#define TOKENS_H_SENTRY

enum { word_size = 256 };
enum { regular_token = 0, separator = 1 };

struct token_item {
	char *word;
    int type;
	struct token_item *next;
};

#endif
