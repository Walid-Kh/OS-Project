#ifndef _PRIORITY_QUEUE_H
#define _PRIORITY_QUEUE_H

// TODO: Make a swap function for process nodes
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

#define Lchild(i) (2 * i + 1)
#define Rchild(i) (2 * i + 2)
#define parent(i) ((i - 1) / 2)

typedef struct minHeap
{
    // TODO: replace with process type
    PCB *arr;
    int count;
    int capacity;
} minHeap;

bool isHeapEmpty(minHeap *heap)
{
    return !heap->count;
}

bool INTERNAL_hasLeftChild(minHeap *heap, int index)
{
    return Lchild(index) < heap->count;
}
bool INTERNAL_hasRightChild(minHeap *heap, int index)
{
    return Rchild(index) < heap->count;
}

struct PCB *Peek(minHeap *heap)
{
     return &(heap->arr[0]); 
}

minHeap *createHeap(int capacity)
{
    minHeap *heap = (minHeap *)malloc(sizeof(minHeap));
    heap->capacity = capacity;
    heap->arr = (PCB *)malloc(sizeof(PCB) * capacity);
}

void destroyHeap(minHeap *heap)
{
    free(heap->arr);
    free(heap);
}

// HPF

void INTERNAL_swap(minHeap *heap, int i1, int i2)
{
    PCB temp = heap->arr[i1];
    heap->arr[i1] = heap->arr[i2];
    heap->arr[i2] = temp;
}

void INTERNAL_heapifyDownHPF(minHeap *heap)
{
    int index = 0;
    while (INTERNAL_hasLeftChild(heap, index))
    {
        int min = Lchild(index);
        if (INTERNAL_hasRightChild(heap, index) && heap->arr[min].priority > heap->arr[Rchild(index)].priority)
            min = Rchild(index);
        if (heap->arr[index].priority <= heap->arr[min].priority)
            break;
        else
        {
            INTERNAL_swap(heap, index, min);
            index = min;
        }
    }
}

void INTERNAL_heapifyUpHPF(minHeap *heap)
{
    int index = heap->count - 1;
    while (index != 0 && heap->arr[index].priority < heap->arr[parent(index)].priority)
    {
        INTERNAL_swap(heap, index, parent(index));
        index = parent(index);
    }
}

// TODO: Replace with process type (return type)
struct PCB *extractHPF(minHeap *heap)
{
    if (heap->count == 0)
    {
        struct PCB r = {0, 0, 0, 0, 0, 0, 0, 0};
        struct PCB *r1 = &r;
        return r1;
    }
    struct PCB item = heap->arr[0];
    heap->arr[0] = heap->arr[heap->count - 1];
    heap->count--;
    INTERNAL_heapifyDownHPF(heap);
    struct PCB *r1 = &item;
    return r1;
}

// TODO: Replace with process type (item)
void insertHPF(minHeap *heap, struct PCB *item)
{
    if (heap->capacity == heap->count)
        return;
    heap->arr[heap->count] = *item;
    heap->count++;
    INTERNAL_heapifyUpHPF(heap);
}

// STRN

void INTERNAL_heapifyDownSTRN(minHeap *heap)
{
    int index = 0;
    while (INTERNAL_hasLeftChild(heap, index))
    {
        int min = Lchild(index);
        if (INTERNAL_hasRightChild(heap, index) && heap->arr[min].remainingTime > heap->arr[Rchild(index)].remainingTime)
            min = Rchild(index);
        if (heap->arr[index].remainingTime <= heap->arr[min].remainingTime)
            break;
        else
        {
            INTERNAL_swap(heap, index, min);
            index = min;
        }
    }
}

void INTERNAL_heapifyUpSTRN(minHeap *heap)
{
    int index = heap->count - 1;
    while (index != 0 && heap->arr[index].remainingTime < heap->arr[parent(index)].remainingTime)
    {
        INTERNAL_swap(heap, index, parent(index));
        index = parent(index);
    }
}

struct PCB *extractSTRN(minHeap *heap)
{
    if (heap->count == 0)
    {
        struct PCB r = {0, 0, 0, 0, 0, 0, 0, 0};
        struct PCB *r1 = &r;
        return r1;
    }
    struct PCB item = heap->arr[0];
    heap->arr[0] = heap->arr[heap->count - 1];
    heap->count--;
    INTERNAL_heapifyDownSTRN(heap);
    struct PCB *r1 = &item;
    return r1;
}

void insertSTRN(minHeap *heap, struct PCB *item)
{
    if (heap->capacity == heap->count)
        return;
    heap->arr[heap->count] = *item;
    heap->count++;
    INTERNAL_heapifyUpSTRN(heap);
}


/* original functions
void INTERNAL_swap(minHeap *heap, int index, int min)
{
    int temp = heap->arr[index];
    heap->arr[index] = heap->arr[min];
    heap->arr[min] = temp;
}

void INTERNAL_heapifyDown(minHeap *heap)
{
    int index = 0;
    while (INTERNAL_hasLeftChild(heap, index))
    {
        int min = Lchild(index);
        if (INTERNAL_hasRightChild(heap, index) && heap->arr[min] > heap->arr[Rchild(index)])
            min = Rchild(index);
        if (heap->arr[index] <= heap->arr[min])
            break;
        else
        {
            int temp = heap->arr[index];
            heap->arr[index] = heap->arr[min];
            heap->arr[min] = temp;
            index = min;
        }
    }
}

void INTERNAL_heapifyUp(minHeap *heap)
{
    int index = heap->count - 1;
    while (index != 0 && heap->arr[index] < heap->arr[parent(index)])
    {
        int temp = heap->arr[index];
        heap->arr[index] = heap->arr[parent(index)];
        heap->arr[parent(index)] = temp;
        index = parent(index);
    }
}

int extract(minHeap *heap)
{
    if (heap->count == 0)
        return 0;
    int item = heap->arr[0];
    heap->arr[0] = heap->arr[heap->count - 1];
    heap->count--;
    INTERNAL_heapifyDown(heap);
    return item;
}

// TODO:  with process type (item)
void insert(minHeap *heap, int item)
{
    if (heap->capacity == heap->count)
        return;
    heap->arr[heap->count] = item;
    heap->count++;
    INTERNAL_heapifyUp(heap);
}*/

#endif