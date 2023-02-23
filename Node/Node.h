#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node_st
{
    struct  node_st* next;
    char*   value;

}Node;

Node*       new_Node(char*);
void        replace_value(Node*, char*);
void        delete_Node(Node*);
void        delete_all_Nodes(Node*);

#endif