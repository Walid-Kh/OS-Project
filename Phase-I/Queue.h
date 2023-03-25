#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


struct Node
{
  int data;
  struct Node* Next;

};

struct Queue
{
   struct Node* Front;
    struct Node* Rear;
    int count;
};

bool isEmpty(struct Queue q) {
    if (q.Front == NULL)
        return 1;
    return 0;
}

void enqueue(struct Queue q,int value){

    struct Node *n ;
    n =(struct Node*) malloc(sizeof(struct Node));

    if(q.count==0)
    {
        q.Front=n;
        q.Rear=n;
    }
    else{

        q.Rear->Next=n;
        q.Rear=n;
        n->data=value;
        q.count=q.count+1;

    }

}


void dequeue(struct Queue q){

   struct Node *temp;
    temp=q.Front;
    q.Front=q.Front->Next;
    free(temp);
    q.count=q.count-1;

}



