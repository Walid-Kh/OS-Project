#include "headers.h"
#include "priorityQueue.h"
#include "circularQueue.h"

int Qid;
int processesCount;
bool isRunning = false;
struct PCB currentRunningProcess;
minHeap *q;
struct circularQueue *Q;
int timeSlice = -1;
int algoNum;

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
    kill(getppid(), SIGINT);
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
        switch (algoNum)
        {
        case 1:
            currentRunningProcess.currentState = FINISHED;
            currentRunningProcess.finishTime = getClk();
            currentRunningProcess.remainingTime = 0;
            currentRunningProcess.turnAroundTime = currentRunningProcess.finishTime - currentRunningProcess.arrivalTime;
            break;
        case 3:
            if (currentRunningProcess.remainingTime >= timeSlice)
            {
                currentRunningProcess.currentState = STOPPED;
                currentRunningProcess.remainingTime -= timeSlice;
                cqEnqueue(Q, &currentRunningProcess);
            }
            else
            {
                currentRunningProcess.currentState = FINISHED;
                currentRunningProcess.remainingTime = 0;
                currentRunningProcess.finishTime = getClk();
                currentRunningProcess.turnAroundTime = currentRunningProcess.finishTime - currentRunningProcess.arrivalTime;
            }

            break;
        }
        writeStats();
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
        while (remainingProcesses > 0 && msgrcv(Qid, &p, sizeof(p.process), 0, IPC_NOWAIT) != -1)
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
        if (!isRunning && !isHeapEmpty(q))
        {
            isRunning = true;
            struct PCB *pcb = extractHPF(q);

            pcb->waitingTime = getClk() - pcb->arrivalTime;
            pcb->currentState = STARTED;
            pcb->remainingTime = pcb->runningTime;
            pcb->startingTime = getClk();
            currentRunningProcess = *pcb;
            writeStats();
            int pid = fork();
            if (pid == -1)
            {
                perror("error forking");
                exit(-1);
            }
            if (pid == 0)
            {
                char remainingTime[4];
                sprintf(remainingTime, "%d", currentRunningProcess.remainingTime);
                execl("./process.out", "process.out", remainingTime, NULL);
                exit(0);
            }
            currentRunningProcess.pid = pid;
        }
    }
}

void RR(int tS)
{
    timeSlice = tS;
    Q = createCircularQueue();

    int remainingProcesses = processesCount;
    while (remainingProcesses || !cqIsEmpty(Q) || isRunning)
    {
        struct processMsg p;
        while (remainingProcesses > 0 && msgrcv(Qid, &p, sizeof(p.process), 0, IPC_NOWAIT) != -1)
        {
            struct PCB pcb;
            pcb.id = p.process.id;
            pcb.priority = p.process.priority;
            pcb.arrivalTime = p.process.arrival;
            pcb.runningTime = p.process.runtime;
            pcb.remainingTime = p.process.runtime;
            pcb.startingTime = -1;
            pcb.waitingTime = -1;
            pcb.finishTime = -1;
            pcb.turnAroundTime = -1;
            pcb.currentState = STOPPED;
            cqEnqueue(Q, &pcb);
            remainingProcesses--;
        }
        if (!isRunning)
        {
            isRunning = true;
            cqDequeue(Q, &currentRunningProcess);
            if (currentRunningProcess.startingTime < 0)
            {
                currentRunningProcess.startingTime = getClk();
                currentRunningProcess.currentState = STARTED;
                currentRunningProcess.waitingTime = getClk() - currentRunningProcess.arrivalTime;
            }
            else
            {
                currentRunningProcess.currentState = RESUMED;
            }

            writeStats();

            int pid = fork();
            if (pid == -1)
            {
                perror("Error in forking");
                exit(-1);
            }
            else if (pid == 0)
            {
                if (currentRunningProcess.remainingTime >= timeSlice)
                {
                    char timeSliceStr[4];
                    sprintf(timeSliceStr, "%d", timeSlice);
                    execl("./process.out", "process", timeSliceStr, (char *)NULL);
                }
                else
                {
                    char remainingTime[4];
                    sprintf(remainingTime, "%d", currentRunningProcess.remainingTime);
                    execl("./process.out", "process", remainingTime, (char *)NULL);
                }
                exit(0);
            }
            currentRunningProcess.pid = pid;
        }
    }
}

int main(int argc, char *argv[])
{
    // TODO implement the scheduler :)
    // upon termination release the clock resources.
    // initResources();

    initClk();
    initFile();
    Qid = msgget(PG_SH_KEY, 0666 | IPC_CREAT);
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
    // algoNum = atoi(argv[1]);
    // processesCount = atoi(argv[2]);
    processesCount = 3;
    algoNum = 3;
    switch (3)
    {
    case 1:
        HPF();
        break;
    case 2:
        // STRN();
        break;
    case 3:
        // timeSlice = atoi(argv[3]);
        RR(2);
        break;
    };

    // clearResources();
}
