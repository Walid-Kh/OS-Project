#include <stdio.h>
#include <stdbool.h>

//Todo make MAX_SIZE is dynamic
#define MAX_SIZE 5

struct circular_queue
{
    int front;
    int back;
    int arr [MAX_SIZE];
    int size;
};

bool isFull(struct circular_queue* q)
{
    if(q->size == MAX_SIZE)
    {
        return true;
    }
    return false;
}

bool isEmpty(struct circular_queue* q)
{
    if(q->size == 0)
    {
        return true;
    }
    return false;
}

bool enqueue(struct circular_queue* q, int element)
{
    if(isFull(q))
    {
        return false;
    }
    q->arr[q->back]=element;
    q->back = (q->back + 1) % MAX_SIZE;
    q->size++;
    return true;
}

bool dequeue(struct circular_queue* q,int* element)
{
    if(isEmpty(q))
    {
        return false;
    }
    *element=q->arr[q->front];
    q->front = (q->front + 1) % MAX_SIZE;
    q->size--;
    return true;
}

bool peek(struct circular_queue* q,int* element)
{
    if(isEmpty(q))
    {
        return false;
    }
    *element=q->arr[q->front];
    return true;
}