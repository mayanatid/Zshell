
#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include <stdbool.h>
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
void    shell_execute_prog(Shell*);
void    shell_destroy(Shell*);

#endif