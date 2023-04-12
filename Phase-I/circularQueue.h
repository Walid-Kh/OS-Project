#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "types.h"

// Todo make MAX_SIZE is dynamic
#define MAX_SIZE 5

typedef struct circularQueue
{
    int front;
    int back;
    struct PCB arr[MAX_SIZE];
    int size;
} circularQueue;

struct circularQueue *createCircularQueue()
{
    struct circularQueue *q;
    q = (struct circularQueue *)malloc(sizeof(circularQueue));
    q->back = 0;
    q->front = 0;
    q->size = 0;
    return q;
};

bool cqIsFull(struct circularQueue *q)
{
    if (q->size == MAX_SIZE)
    {
        return true;
    }
    return false;
}

bool cqIsEmpty(struct circularQueue *q)
{
    if (q->size == 0)
    {
        return true;
    }
    return false;
}

bool cqEnqueue(struct circularQueue *q, struct PCB *p)
{
    if (cqIsFull(q))
    {
        return false;
    }
    q->arr[q->back] = *p;
    q->back = (q->back + 1) % MAX_SIZE;
    q->size++;
    return true;
}

bool cqDequeue(struct circularQueue *q, struct PCB *p)
{
    if (cqIsEmpty(q))
    {
        return false;
    }
    *p = q->arr[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->size--;
    return true;
}

bool cqPeek(struct circularQueue *q, struct PCB *p)
{
    if (cqIsEmpty(q))
    {
        return false;
    }
    *p = q->arr[q->front];
    return true;
}