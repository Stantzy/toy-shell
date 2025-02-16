#ifndef EXEC_STRUCTS_SENTRY_H
#define EXEC_STRUCTS_SENTRY_H

struct exec_options {
    int background, count_pipelines, cmd_pgid;
    int rdir_in_flag, rdir_out_flag, rdir_append_flag;
    int save_stdin, save_stdout;
    int cur_fdin, cur_fdout;
    char *in_path, *out_path;
};

struct cmd_line {
    char **cmdl;
    int pid, pgid;
    int fd_in, fd_out;
    struct cmd_line *next;
};

#endif