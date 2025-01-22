#ifndef EXEC_STRUCTS_SENTRY_H
#define EXEC_STRUCTS_SENTRY_H

struct exec_options {
    int background;
    int redirection_in;
    int redirection_out;
    int redirection_out_rewrite;
    char *in_path;
    char *out_path;
};

struct file_descriptors {
    int fd_out;
    int fd_in;
    int save_stdout;
    int save_stdin;
};

#endif