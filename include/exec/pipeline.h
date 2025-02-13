#ifndef PIPELINE_H_SENTRY
#define PIPELINE_H_SENTRY

int (*create_pipes(struct exec_options opt))[pipe_size];

void make_pipelines(
    struct cmd_line *cl,
    struct exec_options opt,
    int (*ptr_pipe)[2]
);

void process_pipelines(
    int (*ptr_pipe)[2],
    struct cmd_line cl,
    struct exec_options opt
);

#endif