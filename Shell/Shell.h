
#ifndef SHELL_H
#define SHELL_H
#include <stdio.h>
#include "../LinkedList/LinkedList.h"

#define MAX_BUFFER 1024

typedef struct shell_st
{
    char            input[MAX_BUFFER];
    LinkedList*     commands;
    LinkedList*     arguments;
    char**          argList;
    char**          envList;
    void            (*init)(struct shell_st*);
    void            (*listen)(struct shell_st*);
    void            (*parse_input)(struct shell_st*);
    void            (*execute_prog)(struct shell_st*);
    void            (*destroy)(struct shell_st*);
} Shell;

Shell*  new_shell();
void    shell_constructor(Shell*);
void    shell_listen(Shell*);
void    shell_parse_input(Shell*);
void    shell_execute_prog(Shell*);
void    shell_destroy(Shell*);

#endif