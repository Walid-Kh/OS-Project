#include "priorityQueue.h"
#include "stdio.h"
#include "types.h"
int main()
{
    minHeap *h = createHeap(5);
    for (int i = 0; i < 5; i++)
    {
        struct PCB r = {rand() % 10, rand() % 10, rand() % 10, rand() % 10, rand() % 10, rand() % 10, rand() % 10, rand() % 10};
        insertHPF(h, &r);
    }
    for (int i = 0; i < 5; i++)
    {
        struct PCB *res = extractHPF(h);
        printf("\n %d", res->priority);
    }
}