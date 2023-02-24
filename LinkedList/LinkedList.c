#include "LinkedList.h"

void ll_constructor(LinkedList* this)
{
    this->head = NULL;
    this->length = 0;
}


LinkedList* new_ll()
{
    LinkedList template =   {
                                .init       = &ll_constructor,
                                .add        = &ll_add,
                                .print      = &ll_print,
                                .destroy    = &ll_destroy

                            };
    LinkedList* result = malloc(sizeof(LinkedList));
    memcpy(result, &template, sizeof(LinkedList));
    result->init(result);
    return result;
}

void ll_add(LinkedList* this, char* value)
{
    Node* node = new_Node(value);
    if(!this->head)
    {
        this->head = node;
    }
    else
    {
        Node* temp = this->head;
        while(temp->next)
        {
            temp = temp->next;
        }
        temp->next = node;
    }
    this->length++;
}

void ll_print(LinkedList* this)
{
    Node* temp = this->head;
    while(temp->next)
    {
        printf("%s->", temp->value);
        temp = temp->next;
    }
    printf("%s\n", temp->value);
}

void ll_destroy(LinkedList* this)
{
    delete_all_Nodes(this->head);
    free(this);
}