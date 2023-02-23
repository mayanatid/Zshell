#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Node/Node.h"


typedef struct linked_list_st
{
    Node*   head;
    int     length;
    void    (*init)(struct linked_list_st*);
    void    (*add)(struct linked_list_st*, char* value);
    void    (*print)(struct linked_list_st*);
    void    (*destroy)(struct linked_list_st*);

}LinkedList;

LinkedList* new_ll();
void        ll_constructor(LinkedList*);
void        ll_add(LinkedList*, char*);
void        ll_print(LinkedList*);
void        ll_destroy(LinkedList*);

#endif