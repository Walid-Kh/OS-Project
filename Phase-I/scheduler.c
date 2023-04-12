#include "headers.h"
#include "priorityQueue.h"
int Qid;
int processesCount;
bool isRunning = false;
struct PCB currentRunningProcess;
minHeap *q;
void initFile()
{
    FILE *file = fopen("Scheduler.log", "w");
    fprintf(file, "");
    fclose(file);
}
void writeStats()
{
    FILE *file = fopen("Scheduler.log", "a");
    if (currentRunningProcess.currentState != FINISHED)
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d",
                getClk(), currentRunningProcess.id, stateToString(currentRunningProcess.currentState), currentRunningProcess.arrivalTime, currentRunningProcess.runningTime, currentRunningProcess.remainingTime, currentRunningProcess.waitingTime);
    else
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WA %f",
                getClk(),
                currentRunningProcess.id,
                stateToString(currentRunningProcess.currentState),
                currentRunningProcess.arrivalTime,
                currentRunningProcess.runningTime,
                currentRunningProcess.remainingTime,
                currentRunningProcess.waitingTime,
                currentRunningProcess.turnAroundTime,
                round((currentRunningProcess.turnAroundTime / (double)currentRunningProcess.runningTime) * 100) / 100.0f);
    fprintf(file, "\n");
    fclose(file);
}
void clearResources()
{
    destroyClk(true);
    destroyHeap(q);
}
void initResources()
{
    initClk();
    initFile();
    Qid = msgget(PG_SH_KEY, 0666 | IPC_CREAT);
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
}
void handler(int signum)
{

    switch (signum)
    {
    case SIGINT:
        clearResources();
        exit(-1);
        break;
    case SIGUSR2:
        isRunning = false;
        currentRunningProcess.currentState = FINISHED;
        currentRunningProcess.finishTime = getClk();
        currentRunningProcess.remainingTime = 0;
        currentRunningProcess.turnAroundTime = currentRunningProcess.finishTime - currentRunningProcess.arrivalTime;
        writeStats(currentRunningProcess);
        waitpid(currentRunningProcess.pid, (int *)0, 0);
        signal(SIGUSR2, handler);
        break;
    }
}

void HPF()
{

    int remainingProcesses = processesCount;
    q = createHeap(processesCount);
    while (remainingProcesses || isRunning || !isHeapEmpty(q))
    {
        struct processMsg p;
        if (remainingProcesses > 0)
        {
            if ((msgrcv(Qid, &p, sizeof(p.process), 0, !IPC_NOWAIT) == -1))
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
        }
        if (!isRunning && !isHeapEmpty(q))
        {
            isRunning = true;
            struct PCB *pcb = extractHPF(q);

            pcb->waitingTime = getClk() - pcb->arrivalTime;
            pcb->currentState = STARTED;
            pcb->remainingTime = pcb->runningTime;
            pcb->startingTime = getClk();
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
                execl("./process.out", "process.out", remainingTime, NULL);
                exit(0);
            }
            pcb->pid = pid;
            currentRunningProcess = *pcb;
            writeStats();
        }
    }
}

int main(int argc, char *argv[])
{
    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    initResources();

    int algoNum = atoi(argv[1]);
    processesCount = atoi(argv[2]);
    switch (algoNum)
    {
    case 1:
        HPF();
        break;
    case 2:
        // STRN();
        break;
    case 3:
        // RR();
        break;
    };

    clearResources();
}
