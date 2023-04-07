#include "priorityQueue.h"
#include "stdio.h"

int main()
{
    minHeap *h = createHeap(10);
    for (int i = 1; i < 6; i++)
    {
        insert(h, i);
    }
    insert(h, 0);
    for (int i = 1; i < 6; i++)
    {
        printf("%d ", extract(h));
    }
}