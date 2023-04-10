#include "QueueW.h"
int main()
{
    struct Queue *q = createQueue();
    for (int i = 0; i < 5; i++)
    {
        struct process x = {i, rand() % 4, rand() % 4, rand() % 4};
        enqueue(q, &x);
    }
    for (int i = 0; i < 5; i++)
    {
        struct process x = dequeue(q);
        printf("%d ", x.id);
    }
}