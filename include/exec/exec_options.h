#ifndef EXEC_OPTIONS_H_SENTRY
#define EXEC_OPTIONS_H_SENTRY

void init_options(struct exec_options *o);
int update_options(struct token_item *first, struct exec_options *o);

#endif