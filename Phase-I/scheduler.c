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
minHeap *pq;
processMes p;


float avgWTA=0;
float avgWaiting=0;
float* WTAarr;  // using in std calc
int arrcount=0;
float totalTime=0;
float totalRunningtime=0;




void initFile()
{
    FILE *file = fopen("Scheduler.log", "w");
    // fprintf(file,"");
    fclose(file);
    file= fopen("scheduler.perf", "w");
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
    switch (algoNum)
    {
    case 1:
        destroyHeap(q);
        break;
    case 2:
        destroyHeap(pq);
        break;
    case 3:
        destroyCQ(Q);
        break;
    }

    kill(getppid(), SIGINT);
}
void writePerf()
{
    FILE *file = fopen("scheduler.perf", "w");
    float uti =(totalRunningtime/totalTime)*100;
    fprintf(file, "CPU utilization = %f %% \n",uti);
    fprintf(file,"Avg WTA = %f \n",avgWTA/processesCount);
    fprintf(file,"Avg Waiting = %f \n",avgWaiting/processesCount);
    //calc Std
    float std =0;
    for (int i = 0; i < processesCount; ++i)
        std+= pow((WTAarr[i]-(avgWTA/processesCount)),2);
    std=sqrt(std/(processesCount-1));

    fprintf(file,"Std WTA = %f",std);

    fclose(file);
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
            if (currentRunningProcess.remainingTime > timeSlice)
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
        case 2:
            currentRunningProcess.currentState = FINISHED;
            currentRunningProcess.finishTime = getClk();
            currentRunningProcess.remainingTime = 0;
            currentRunningProcess.turnAroundTime = currentRunningProcess.finishTime - currentRunningProcess.arrivalTime;
            break;
        }
        WTAarr[arrcount]= round((currentRunningProcess.turnAroundTime / (double)currentRunningProcess.runningTime) * 100) / 100.0f;
        avgWTA +=WTAarr[arrcount++];
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
            totalRunningtime+=pcb.runningTime;
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
            avgWaiting+=currentRunningProcess.waitingTime;
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
void SRTN()
{
    int num = processesCount;
    pq = createHeap(num);
    while (num > 0 || pq->count > 0 || isRunning)
    {
       // printf("num is%d\n count is%d\nis running=%d\n", num, pq->count, isRunning);
        bool received_now = false;
        while (num > 0 && msgrcv(Qid, &p, sizeof(p.process), 0, IPC_NOWAIT) != -1)
        {
            struct PCB *curr;
            num--;
            received_now = true;
            curr->arrivalTime = p.process.arrival;
            curr->id = p.process.id;
            curr->priority = p.process.priority;
            curr->runningTime = p.process.runtime;
            curr->remainingTime = curr->runningTime;
            totalRunningtime+=curr->runningTime;
            insertSTRN(pq, curr);
        }
        if (isRunning == true && received_now)
        {
            int t = getClk() - currentRunningProcess.startingTime;
            currentRunningProcess.remainingTime = currentRunningProcess.runningTime - t;
            if (currentRunningProcess.remainingTime > p.process.runtime)
            {
                kill(currentRunningProcess.pid, SIGINT);
                //   printf("process Preempted With id=%d", currentRunningProcess.pid);
                //    printf(" Process With Id=%d Preempted At%d=\n", currentRunningProcess.id, getClk());
                currentRunningProcess.currentState = STOPPED;
                writeStats();
                insertSTRN(pq, &currentRunningProcess);
                isRunning = false;
            }
        }
        if (isRunning == false && pq->count > 0)
        {
            PCB *go = extractSTRN(pq);
            isRunning = true;                         // Logic Of The Algorithm
            if (go->remainingTime != go->runningTime) // Meant That This Is Preempted Process
            {
                go->currentState = RESUMED;
            }
            else
            {
                go->startingTime = getClk();
                go->currentState = STARTED;
                go->waitingTime = getClk() - go->arrivalTime;
                avgWaiting+=go->waitingTime;
            }
            currentRunningProcess = *go;
            writeStats();
            int pid = fork();
            if (pid == 0)
            {
                char remainingTime[4];
                sprintf(remainingTime, "%d", currentRunningProcess.remainingTime);
                execl("./process.out", "./process.out", remainingTime, NULL);
                exit(0);
            }
            else
            {
                currentRunningProcess.pid = pid;
            }
        }
    }
}
void RR(int tS)
{
    timeSlice = tS;
    Q = createCircularQueue(processesCount);

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
            totalRunningtime+=pcb.runningTime;
            cqEnqueue(Q, &pcb);
            remainingProcesses--;
        }
        if (!isRunning && !cqIsEmpty(Q))
        {
            isRunning = true;
            cqDequeue(Q, &currentRunningProcess);
            if (currentRunningProcess.startingTime < 0)
            {
                currentRunningProcess.startingTime = getClk();
                currentRunningProcess.currentState = STARTED;
                currentRunningProcess.waitingTime = getClk() - currentRunningProcess.arrivalTime;
                avgWaiting+=currentRunningProcess.waitingTime;
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
    algoNum = atoi(argv[1]);
    processesCount = atoi(argv[2]);
    WTAarr=(float*) malloc(processesCount*sizeof(float)); //the array of  WTA

    printf("Num Is=%d", processesCount);
  //  processesCount += 1;
    // processesCount = 5;
    // algoNum = 2;
      fflush(stdout);
    switch (algoNum)
    {
    case 1:
        HPF();
        break;
    case 2:
        SRTN();
        break;
    case 3:
        timeSlice = atoi(argv[3]);
        RR(timeSlice);
        break;
    };
    totalTime=getClk();
    writePerf();
    destroyClk(false);
    kill(getppid(), SIGINT);
    return 0;
    // clearResources();
}
