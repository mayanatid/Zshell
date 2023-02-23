#include "Node.h"

Node*   new_Node(char* val)
{
    Node* m_node = malloc(sizeof(Node));
    m_node->value = (char*)malloc(strlen(val) + 1);
    memset(m_node->value, 0 , strlen(val) + 1);
    strcpy(m_node->value, val);
    m_node->next = NULL;
    return m_node;
}

void    replace_value(Node* this, char* new_val)
{
    this->value = (char*)realloc(this->value, strlen(new_val) + 1);
    memset(this->value, 0, strlen(new_val) + 1);
    strcpy(this->value, new_val);
}

void    delete_Node(Node* this)
{
    free(this->value);
    free(this);
}

void    delete_all_Nodes(Node * this)
{
    Node* temp = this;
    while(this)
    {
        this = this->next;
        delete_Node(temp);
        temp = this;
    }
}