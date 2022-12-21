#include <stdlib.h>
#include <stdio.h>
#include "myqueue.h"

node_t *head = NULL;
node_t *tail = NULL;

//tms - time in ms, iternum - iteration number, c0r1 - 0 for cubic and 1 for reno.
void enqueue(long int *tms, int *iternum, int *c0r1)
{
    node_t *newnode = malloc(sizeof(node_t));
    newnode->tms = tms;
    newnode->iternum = iternum;
    newnode->c0r1 = c0r1;
    newnode->next = NULL;
    if (tail == NULL)
    {
        head = newnode;
    }
    else
    {
        tail->next = newnode;
    }
    tail = newnode;
}

// return the value
long int *dequeue()
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        long int *result = head->tms;
        if(*(head->c0r1) == 0)
        {
            printf("Algorithm: cubic, iteration number: %d, time: %ld (ms)\n",
          *(head->iternum), *(head->tms));
        }
        else if(*(head->c0r1) == 1){
            printf("Algorithm: reno, iteration number: %d, time: %ld (ms)\n",
          *(head->iternum), *(head->tms));
        }

        node_t *temp = head;
        head = head->next;
        if (head == NULL)
        {
            tail = NULL;
        }
        free(temp);
        return result;
    }
}