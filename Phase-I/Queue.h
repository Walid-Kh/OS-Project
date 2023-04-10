#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>




struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
};


struct Node
{
    struct process process;
    struct Node * Next;

};

struct Queue
{
   struct Node* Front;
   struct Node* Rear;
   int count;
};

void  queueConstractor(struct Queue q)
{
    q.Front=NULL;
    q.Rear=NULL;
    q.count=0;
}



bool isEmpty(struct Queue q) {
    if (q.Front == NULL)
        return 1;
    return 0;
}

void enqueue(struct Queue q,struct process p){

    struct Node *n ;
    n =(struct Node*) malloc(sizeof(struct Node));

    n->process=p;

    if(q.count==0)
    {
        q.Front=n;
        q.Rear=n;
        n->process=p;
    }

    else{
        q.Rear->Next=n; //there is a error here
        q.Rear=n;
        q.count=q.count+1;

     }

}


struct process dequeue (struct Queue q){

   struct Node *temp;
   struct process tempProcess;
    tempProcess=q.Front->process;

    temp=q.Front;
    q.Front=q.Front->Next;
    free(temp);
    q.count=q.count-1;
    return tempProcess;

}



