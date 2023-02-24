
#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../LinkedList/LinkedList.h"

#define MAX_BUFFER 1024

typedef struct shell_st
{
    bool            process;
    char            input[MAX_BUFFER];
    LinkedList*     commands;
    LinkedList*     arguments;
    char**          argList;
    char**          envList;
    void            (*init)(struct shell_st*);
    int             (*read_input)(struct shell_st*);
    int             (*listen)(struct shell_st*);
    void            (*parse_commands)(struct shell_st*);
    void            (*parse_args)(struct shell_st*, char*);
    void            (*parse_input)(struct shell_st*);
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
void    helper_sub_env_vars(Shell*);
char*   helper_find_path_in_env(Shell*);
bool    helper_cmd_in_dir(char, char*);
char*   helper_read_path(char*, char*);
void    helper_exec_pwd(Shell*);
void    helper_exec_cd(Shell*);
void    helper_exec_setenv(Shell*);
void    helper_exec_unsetenv(Shell*);
void    helper_exec_which(Shell*);



#endif