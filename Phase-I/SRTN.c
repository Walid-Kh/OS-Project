#include "headers.h"
#include <stdio.h>
#include "priorityQueue.h"
#include <test_generator.c>
minHeap *pq;
int msgqid;
bool Finished_Receving = 0; // For Communication With Process Generator To Send Processes Data
typedef struct processData Process;
Process *curr_id = 0;
void IntializeMessage_Queue()
{
    _key_t key;
    Key = ftok("Keytest.txt", 100);
    msgqid = msgget(key, 0666 | IPC_CREAT);
}
void CheckComingProcess()
{
    Process p;
    int recv_val = msgrcv(msgqid, &p, sizeof(p), 0, IPC_NOWAIT); // Type May Be Added
                                                                 //   if(Some Sort Of Message To Indicate That Receving Has Finished )
                                                                 //   Finished_Receving=1;
    insert(pq, p);                                               // Type of PriorityQueue To Be Modified To Proxess
}
void Start_NewProcess(Process *curr)
{

    int pid = fork();
    if (pid == -1)
        printf("Error In Forking") else if (pid == 0)
        {
            int t = getClk();
            curr->startingtime = t;
            curr->Is_started = 1;
        }
    else
    {
        curr->id = pid;
    }
}
void Begin_SRTN() // Function To Be Called By Process_Generator
{
    IntializeMessage_Queue();
    while (pq->count > 0 || !Finished_Receving)
    {
        if (!Finished_Receving)
            CheckComingProcess();
        if (pq->count)
        {
            // Peek Function Not Extract
            Process *procs = Peek(pq);
            if (curr_id == 0) // No Running Process
            {
                if (!procs->Is_started)
                {
                    procs = extract(pq);
                    Start_NewProcess(procs);
                }
                else
                { 
                    //Load Its State And Continue Process
                }
            }
            else // There Is Already Currrent Running Process
            {
            }
        }
    }
}
int main()
{
    printf("Hello Iam Manga");
}