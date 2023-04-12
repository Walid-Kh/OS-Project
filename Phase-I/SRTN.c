#include "headers.h"
#include <stdio.h>
#include "priorityQueue.h"
minHeap *pq;
int msgqid;
bool Finished_Receving = 0; // For Communication With Process Generator To Send Processes Data
PCB *curr_Process_pcb = NULL;
processMes p;
bool Finished = false;
void OutputFile(int time)
{
    FILE *file = fopen("Scheduler.log", "w");
    if (curr_Process_pcb->currentState != FINISHED)
    {
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d",
                time, curr_Process_pcb->id, stateToString(curr_Process_pcb->currentState), curr_Process_pcb->arrivalTime, curr_Process_pcb->runningTime, curr_Process_pcb->remainingTime, curr_Process_pcb->waitingTime);
    }
    else
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WA %f",
                time,
                curr_Process_pcb->id,
                stateToString(curr_Process_pcb->currentState),
                curr_Process_pcb->runningTime,
                curr_Process_pcb->arrivalTime,
                curr_Process_pcb->remainingTime,
                curr_Process_pcb->waitingTime,
                curr_Process_pcb->turnAroundTime,
                /*round*/ ((curr_Process_pcb->turnAroundTime / (double)curr_Process_pcb->runningTime) * 100) / 100.0f);
    fclose(file);
}
void handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        msgctl(msgqid, IPC_RMID, (struct msqid_ds *)0);
        exit(0);
        break;
    case SIGUSR2:
        curr_Process_pcb->currentState = FINISHED;
        curr_Process_pcb->finishTime = getClk();
        curr_Process_pcb->remainingTime = 0;
        curr_Process_pcb->turnAroundTime = curr_Process_pcb->finishTime - curr_Process_pcb->arrivalTime;
        // OutputFile(curr_Process_pcb->finishTime);
        printf("Insiede Handler");
        fflush(stdout);
        curr_Process_pcb = NULL;
        break;
    }
}
void IntializeMessage_Queue()
{
    // __key_t key;
    // key = ftok("Keytest.txt", 100);
    msgqid = msgget(PG_SH_KEY, 0666 | IPC_CREAT);
}
void Begin_SRTN(int numofprocess) // Function To Be Called By Process_Generator
{
    IntializeMessage_Queue();
    int num = numofprocess;
    pq = createHeap(num);
    PCB *curr = malloc(sizeof(PCB));
    while (pq->count > 0 || !Finished)
    {
        bool received_now = false;
        if (num > 0)
        {
            int recv_val = msgrcv(msgqid, &p, sizeof(p.process), 0, IPC_NOWAIT);
            if (recv_val == -1)
                printf("Erorr in Receiving");
            else
            {
                num--;
                received_now = true;
                curr->arrivalTime = p.process.arrival;
                curr->id = p.process.id;
                curr->priority = p.process.priority;
                curr->runningTime = p.process.runtime;
                curr->remainingTime = curr->runningTime;
                curr->waitingTime = 0;
                if (curr_Process_pcb == NULL || p.process.runtime >= curr_Process_pcb->remainingTime)
                    insertHPF(pq, curr);
            }
        }
        else
            Finished = true;
        if (curr_Process_pcb != NULL && received_now && curr_Process_pcb->remainingTime > p.process.runtime)
        {
            kill(curr_Process_pcb->pid, SIGINT);
            int p = getClk() - curr_Process_pcb->startingTime;
            curr_Process_pcb->remainingTime = curr_Process_pcb->runningTime - p;
            curr_Process_pcb->currentState = STOPPED;
            insertHPF(pq, curr_Process_pcb);
            curr_Process_pcb = NULL;
        }
        if (curr_Process_pcb == NULL)
        {
            PCB *go = extractHPF(pq);
            // Logic Of The Algorithm
            if (go->remainingTime != go->runningTime) // Meant That This Is Preempted Process
            {
                go->currentState = RESUMED;
                // Waiting Time
            }
            go->currentState = STARTED;
            go->startingTime = getClk();
            curr_Process_pcb = go;
            int pid = fork();
            if (pid == -1)
                printf("Error In Forking");
            else if (pid == 0)
            {
                printf("In");
                execl("./process.out", "./process.out", go->runningTime, NULL);
                printf("OUt");
                kill(getppid(), SIGUSR2);
                exit(0);
            }
            else
            {
                go->pid = pid;
            }
        }
    }
}
int main()
{
    printf("Hello Iam Manga");
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
    initClk();
    printf("Iam Started At time=%d", getClk());
    Begin_SRTN(5);
    printf("\nIam Finished At time=%d", getClk());
}