#ifndef TOKENIZER_H_SENTRY
#define TOKENIZER_H_SENTRY

void release_memory(struct token_item *first);
struct token_item *tokenize_string(const char *string);

#endif