#include "headers.h"
#include "types.h"
#include "priorityQueue.h"
int Qid;
int nprocesses;
bool isRunning = false;
struct PCB currentRunningProcess;

void writeStats(struct PCB pcb, int atTime)
{
    FILE *file = fopen("Scheduler.log", "w");
    if (pcb.currentState != FINISHED)
    {
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d",
                atTime, pcb.id, stateToString(pcb.currentState), pcb.arrivalTime, pcb.runningTime, pcb.remainingTime, pcb.waitingTime);
    }
    else
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WA %f",
                atTime,
                pcb.id,
                stateToString(pcb.currentState),
                pcb.arrivalTime,
                pcb.runningTime,
                pcb.remainingTime,
                pcb.waitingTime,
                pcb.turnAroundTime,
                /*round*/((pcb.turnAroundTime / (double)pcb.runningTime) * 100) / 100.0f);
    fclose(file);
}

void handler(int signum)
{

    printf("USR2");
    fflush(stdout);
    switch (signum)
    {
    case SIGINT:
        msgctl(Qid, IPC_RMID, (struct msqid_ds *)0);
        exit(0);
        break;
    case SIGUSR2:
        isRunning = false;
        currentRunningProcess.currentState = FINISHED;
        currentRunningProcess.finishTime = getClk();
        currentRunningProcess.remainingTime = 0;
        currentRunningProcess.turnAroundTime = currentRunningProcess.finishTime - currentRunningProcess.arrivalTime;
        writeStats(currentRunningProcess, currentRunningProcess.finishTime);
        break;
    }
}

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
            PCB pcb;
            insertHPF(q, &pcb);
            remainingProcesses--;
            pcb.arrivalTime = p.process.arrival;
            pcb.id = p.process.id;
            pcb.priority = p.process.priority;
            pcb.runningTime = p.process.runtime;
            pcb.remainingTime = p.process.runtime;
            pcb.currentState = STOPPED;
        }
        if (!isRunning)
        {
        isRunning = true;
        struct PCB *pcb = extractHPF(q);

            pcb->waitingTime = getClk() - pcb->arrivalTime;
            pcb->currentState = STARTED;
            pcb->remainingTime = pcb->runningTime;
            pcb->startingTime = getClk();
            currentRunningProcess = *pcb;
            writeStats(currentRunningProcess, currentRunningProcess.arrivalTime);

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
                exit(0);
            }
            pcb->pid = pid;
       }
    }
}




int main(int argc, char *argv[])
{

    Qid =msgget(PG_SH_KEY, 0666|IPC_CREAT);
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
    initClk();
    nprocesses = 5;
    HPF();
    // TODO implement the scheduler :)
    // upon termination release the clock resources.

    // destroyClk(false);
}
