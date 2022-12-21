#ifndef MYQUEUE_H_
#define MYQUEUE_H_

struct node
{
    struct node *next;
    long int *tms;
    int *iternum;
    int *c0r1;
};

typedef struct node node_t;
void enqueue(long *tms, int *iternum, int *c0r1);
long* dequeue();


extern node_t *head;
extern node_t *tail;

#endif