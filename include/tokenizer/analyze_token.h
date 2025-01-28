#ifndef ANALYZE_TOKEN_H_SENTRY
#define ANALYZE_TOKEN_H_SENTRY

int is_separator(char c);
char *read_word(const char *string, int *offset);
char *handle_separator(const char *str, int *offset);

#endif