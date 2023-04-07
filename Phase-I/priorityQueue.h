// TODO: Make a swap function for process nodes
#include <stdlib.h>
#include <stdbool.h>

#define Lchild(i) (2 * i + 1)
#define Rchild(i) (2 * i + 2)
#define parent(i) ((i - 1) / 2)

typedef struct minHeap
{
    // TODO: replace with process type
    int *arr;
    int count;
    int capacity;
} minHeap;

minHeap *createHeap(int capacity)
{
    minHeap *heap = (minHeap *)malloc(sizeof(minHeap));
    heap->capacity = capacity;
    heap->arr = (int *)malloc(sizeof(int) * capacity);
}
void destroyHeap(minHeap *heap)
{
    free(heap->arr);
    free(heap);
}
bool INTERNAL_hasLeftChild(minHeap *heap, int index)
{
    return Lchild(index) < heap->count;
}
bool INTERNAL_hasRightChild(minHeap *heap, int index)
{
    return Rchild(index) < heap->count;
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

// TODO: Replace with process type (return type)
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

// TODO: Replace with process type (item)
void insert(minHeap *heap, int item)
{
    if (heap->capacity == heap->count)
        return;
    heap->arr[heap->count] = item;
    heap->count++;
    INTERNAL_heapifyUp(heap);
}
struct processData *peek(minHeap *heap)
{
    // return heap->arr[0]; Type Of Arr To Be Modified
}
