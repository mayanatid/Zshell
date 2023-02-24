
#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "../LinkedList/LinkedList.h"



#define MAX_BUFFER 1024

typedef struct shell_st
{
    pid_t           pid;
    int             status;
    bool            process;
    char            prompt[MAX_BUFFER];
    char            input[MAX_BUFFER];
    char            cwd_buffer[MAX_BUFFER];
    char            temp_buffer[MAX_BUFFER];
    char*           prog_path;
    LinkedList*     commands;
    LinkedList*     arguments;
    char**          argList;
    char**          envList;
    void            (*init)(struct shell_st*);
    int             (*read_input)(struct shell_st*);
    int             (*listen)(struct shell_st*);
    void            (*parse_commands)(struct shell_st*);
    void            (*parse_args)(struct shell_st*, char*);
    void            (*parse_input)(struct shell_st*); // unused currently
    bool            (*execute_built_in)(struct shell_st*);
    void            (*execute_prog)(struct shell_st*);
    void            (*destroy)(struct shell_st*);
} Shell;

Shell*  new_shell(char**);
void    shell_constructor(Shell*);
int     shell_read_input(Shell*);
int     shell_listen(Shell*);
void    shell_parse_commands(Shell*);
void    shell_parse_args(Shell*, char*);
void    shell_parse_input(Shell*);
bool    shell_execute_built_in(Shell*);
void    shell_execute_prog(Shell*);
void    shell_destroy(Shell*);

// helpers
void    helper_construct_arg_list(Shell*);
void    helper_destroy_arg_list(Shell*);
void    helper_sub_env_vars(Shell*);
char*   helper_find_path_in_env(Shell*);
char*   helper_construct_env_string(Shell*);
bool    helper_cmd_in_dir(char*, char*);
char*   helper_read_path(char*, char*);
void    helper_exec_pwd(Shell*);
void    helper_exec_cd(Shell*);
void    helper_exec_setenv(Shell*);
void    helper_exec_unsetenv(Shell*);
void    helper_exec_which(Shell*);
void    helper_exec_env(Shell*);



#endif