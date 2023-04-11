#include "headers.h"
#include "types.h"
#include "priorityQueue.h"
int Qid;
int nprocesses;
bool isRunning = false;

void HPF()
{
    int remainingProcesses = nprocesses;
    minHeap *q = createHeap(nprocesses);
    while (remainingProcesses || isRunning)
    {
        struct processMsg p;
        if (msgrcv(Qid, &p, sizeof(p.process), 0, !IPC_NOWAIT) == -1)
        {
            perror("error in receiving");
        }
        else
        {
            remainingProcesses--;
            PCB pcb;
            pcb.arrivalTime = p.process.arrival;
            pcb.id = p.process.id;
            pcb.priority = p.process.priority;
            pcb.runningTime = p.process.runtime;
            pcb.remainingTime = p.process.runtime;
            pcb.currentState = STOPPED;
            insertHPF(q, &pcb);
        }
        if (!isRunning)
        {
            isRunning = true;
            struct PCB *pcb = extractHPF(q);
            pcb->waitingTime = getClk() - pcb->arrivalTime;

            pcb->currentState = STARTED;
            pcb->remainingTime = pcb->runningTime;
            int pid = fork();
            if (pid == -1)
            {
                perror("error forking");
                exit(-1);
            }
            if (pid == 0)
            {
                pcb->pid = getpid();
                char remainingTime[4];
                sprintf(remainingTime, "%d", pcb->remainingTime);
                execl("./process.out", "./process.out", remainingProcesses, NULL);
            }
            pcb->pid = pid;
        }
    }
}
void handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        msgctl(Qid, IPC_RMID, (struct msqid_ds *)0);
        exit(0);
        break;
    case SIGUSR2:
        isRunning = false;
        signal(SIGUSR2, handler);
        break;
    }
}
int main(int argc, char *argv[])
{
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
    initClk();
    key_t qkey = ftok("qkey", 'q');
    Qid = msgget(qkey, 0666 | IPC_CREAT);

    nprocesses = 5;
    HPF();
    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // destroyClk(false);
}
