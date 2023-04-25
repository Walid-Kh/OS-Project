#include "headers.h"
#include "priorityQueue.h"
#include "circularQueue.h"

int Qid;
int processesCount;
bool isRunning = false;
struct PCB currentRunningProcess;
minHeap *q;
int algoNum;
int policy;
minHeap *pq;
processMes p;
int memory[1024] = {0}; // zero mean that the location is free/ id means that the process with this id take this location
int count = 1;
struct circularQueue *Q;
int timeSlice = -1;
int sliceStartTime;
float avgWTA = 0;
float avgWaiting = 0;
float *WTAarr; // using in std calc
int arrcount = 0;
float totalTime = 0;
float totalRunningtime = 0;
void intiallization()
{
    if (policy == 1)
        return;
    for (int i = 0; i < 1024; i++)
        memory[i] = -1 * count;
    count++;
}
bool IsPower(int n)
{
    if (n == 1)
        return false;
    int cnt = 0;
    for (int i = 0; i < 11; i++)
        if (n & (1 << i))
            cnt++;
    return (cnt == 1);
}
void initFile()
{
    FILE *file = fopen("Scheduler.log", "w");
    // fprintf(file,"");
    fclose(file);

    file = fopen("scheduler.perf", "w");
    fclose(file);

    file = fopen("memory.log", "w"); // memory.log
    fclose(file);
}
void writeStats()
{
    FILE *file = fopen("Scheduler.log", "a");
    double WTA;
    if (currentRunningProcess.runningTime != 0)
        WTA = round((currentRunningProcess.turnAroundTime / (double)currentRunningProcess.runningTime) * 100) / 100.0f;
    if (currentRunningProcess.currentState != FINISHED)
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d",
                getClk(), currentRunningProcess.id, stateToString(currentRunningProcess.currentState), currentRunningProcess.arrivalTime, currentRunningProcess.runningTime, currentRunningProcess.remainingTime, currentRunningProcess.waitingTime);
    else
    {
        fprintf(file, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d",
                getClk(),
                currentRunningProcess.id,
                stateToString(currentRunningProcess.currentState),
                currentRunningProcess.arrivalTime,
                currentRunningProcess.runningTime,
                currentRunningProcess.remainingTime,
                currentRunningProcess.waitingTime,
                currentRunningProcess.turnAroundTime);
        if (currentRunningProcess.runningTime == 0)
            fprintf(file, " WA Undefined value");
        else
            fprintf(file, " WA %f ", WTA);
    }
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
    float uti = (totalRunningtime / totalTime) * 100;
    fprintf(file, "CPU utilization = %f %% \n", uti);
    fprintf(file, "Avg WTA = %f \n", avgWTA / processesCount);
    fprintf(file, "Avg Waiting = %f \n", avgWaiting / processesCount);
    // calc Std
    float std = 0;
    for (int i = 0; i < processesCount; ++i)
        std += pow((WTAarr[i] - (avgWTA / processesCount)), 2);
    std = sqrt(std / (processesCount - 1));

    fprintf(file, "Std WTA = %f", std);

    fclose(file);
}
void writeMemory(int start)
{
    FILE *file = fopen("memory.log", "a");
    fprintf(file, "At time %d allocated %d bytes for process %d from %d to %d\n",
            getClk(),
            currentRunningProcess.memsize,
            currentRunningProcess.id,
            start,
            start + currentRunningProcess.memsize - 1);

    fclose(file);
}
void freeMemory(int start, int size)
{

    FILE *file = fopen("memory.log", "a");
    if (size != currentRunningProcess.memsize)
        fprintf(file, "At time %d Bytes From %d To %d Was Combined\n", getClk(), start, start + size);
    else
    {
        fprintf(file, "At Time %d Freed %d Bytes For Process %d From %d To %d\n",
                getClk(),
                size,
                currentRunningProcess.id,
                start,
                start + size - 1);
    }
    fclose(file);
}
bool allocate()
{
    switch (policy)
    {
    case 1:
        int id = currentRunningProcess.id;
        int size = currentRunningProcess.memsize;
        int i, j;
        bool reserved = false;
        bool flag = true;
        for (i = 0; i < 1024; ++i)
        {
            if (memory[i] == 0) // first location available
            {
                for (j = i; j < size; ++j)
                { // check if there enough size
                    if (memory[j] != 0)
                    {
                        flag = false;
                        break;
                    }
                }
                if (!flag)
                    i = j; // there is not enough size
                else
                { // enough size
                    for (int k = i; k < size + i; ++k)
                        memory[k] = id;
                    writeMemory(i);
                    reserved = true;
                    break;
                }
            }
        }
        return reserved;
        break;
    case 2:
        int sz = currentRunningProcess.memsize;
        int mask = 0;
        while ((1 << mask) < sz)
            mask++;
        int go = 1 << mask;
        i = 0;
        int cnt = 1e5; // Size of the segement to be assigned
        int s;         // Start of that Segement
        for (; i < 1024; i++)
        {
            int cnt2 = 0;
            int s2 = i;
            while (i < 1023 && memory[i] < 0 && memory[i + 1] == memory[i])
                i++, cnt2++;
            if (cnt2 + 1 >= sz && cnt2 + 1 < cnt)
            {
                cnt = cnt2;
                s = s2;
            }
        }
        if (cnt == 1e5)
            return false;
        currentRunningProcess.memsize = 1 << mask;
        currentRunningProcess.memstart = s;
        j = s;
        int ok = 0;
        int mul = -1;
        i = s + cnt + 1;
        while (j < i)
        {
            for (int k = 0; k < go; j++, k++)
            {
                if (!ok)
                    memory[j] = currentRunningProcess.id;
                else
                    memory[j] = mul * count;
            }
            if (ok)
                count++;
            ok++;
            if (ok > 1)
            {
                mask++;
                go = 1 << mask;
            }
        }
        writeMemory(currentRunningProcess.memstart);
        return true;
        break;
    default:
        break;
    }
}
void deallocate()
{
    int id = currentRunningProcess.id;
    int sz = currentRunningProcess.memsize;
    int i = 0;
    int s = currentRunningProcess.memstart;
    switch (policy)
    {
    case 1:
        while (memory[i] != id)
            i++;
        freeMemory(i, currentRunningProcess.memsize);
        for (int j = i; j < i + sz; ++j)
            memory[j] = 0;
        break;
    case 2:
        for (int i = s; i < s + sz; i++)
            memory[i] = -1 * count;
        freeMemory(s, sz);
        bool f = true;
        while (f)
        {
            f = false;
            int tmp = sz;
            int mask = 0;
            while (tmp >> 1)
            {
                mask++;
                tmp = tmp >> 1;
            }
            bool l = false, r = false;
            if (s - sz < 0)
                r = true;
            if (s + sz + 1 > 1024)
                l = true;
            // try to merge with left
            if (!l && !r && (IsPower(sz + s) || IsPower(1024 - (s - sz))))
                l = true; // go left
            // try to merge with right
            if (!l && !r && (IsPower(s + 2 * sz) || IsPower(1024 - s)))
                r = true; // go right
                          /*/    if (l && r)
                              {
                                  printf("Impossible");
                              }*/
            if (l)
            {
                f = true;
                int idx = s;
                for (int i = 0; i < sz; i++, idx--)
                    if (memory[idx] > 0 || memory[idx] != memory[idx])
                    {
                        f = false;
                        break;
                    }
                if (f)
                {
                    idx = s - 1;
                    for (int i = 0; i < sz; i++, idx--)
                        memory[idx] = -1 * count;
                    s -= sz;
                    mask++;
                    sz = 1 << mask;
                    freeMemory(s, sz);
                }
            }
            else
            {
                int idx = s + sz;
                f = true;
                for (int i = 0; i < sz; i++, idx++)
                    if (memory[idx] > 0 || memory[idx] != memory[idx])
                    {
                        f = false;
                        break;
                    }
                if (f)
                {
                    idx = s + sz;
                    for (int i = 0; i < sz; i++, idx++)
                        memory[idx] = -1 * count;
                    mask++;
                    sz = 1 << mask;
                    freeMemory(s, sz);
                }
            }
        }
        count += 1;
        break;
    default:
        break;
    }
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

        WTAarr[arrcount] = /*round*/ ((currentRunningProcess.turnAroundTime / (double)currentRunningProcess.runningTime) * 100) / 100.0f;
        avgWTA += WTAarr[arrcount++];
        writeStats();
        deallocate();
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
            pcb.memsize = p.process.memsize;
            totalRunningtime += pcb.runningTime;
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
            avgWaiting += currentRunningProcess.waitingTime;
            allocate();
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
            struct PCB curr;
            num--;
            received_now = true;
            curr.arrivalTime = p.process.arrival;
            curr.id = p.process.id;
            curr.priority = p.process.priority;
            curr.runningTime = p.process.runtime;
            curr.remainingTime = curr.runningTime;
            curr.memsize = p.process.memsize;
            totalRunningtime += curr.runningTime;
            insertSTRN(pq, &curr);
        }
        if (isRunning == true && received_now)
        {
            int t = getClk() - currentRunningProcess.startingTime;
            currentRunningProcess.remainingTime = currentRunningProcess.runningTime - t;
            currentRunningProcess.preemptedTime = getClk();
            if (currentRunningProcess.remainingTime > p.process.runtime)
            {
                kill(currentRunningProcess.pid, SIGSTOP);
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
            /*/  if (!allocate())
              {
                  insertSTRN(pq,go);
                  printf("Not Enought Memory");
                  fflush(stdout);
                  continue;
              }*/
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
                avgWaiting += go->waitingTime;
            }
            currentRunningProcess = *go;
            if (currentRunningProcess.remainingTime != currentRunningProcess.runningTime)
            {
                currentRunningProcess.waitingTime += getClk() - currentRunningProcess.preemptedTime;
                kill(currentRunningProcess.pid, SIGCONT);
                writeStats();
            }
            else
            {
                writeStats();
                allocate();
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
            pcb.memsize = p.process.memsize;
            totalRunningtime += pcb.runningTime;
            cqEnqueue(Q, &pcb);
            remainingProcesses--;
        }
        if (isRunning && (getClk() - sliceStartTime >= timeSlice))
        {
            if (currentRunningProcess.remainingTime > timeSlice)
            {
                isRunning = false;
                kill(currentRunningProcess.pid, SIGSTOP);
                currentRunningProcess.currentState = STOPPED;
                currentRunningProcess.remainingTime -= timeSlice;
                currentRunningProcess.preemptedTime = getClk();
                cqEnqueue(Q, &currentRunningProcess);
                writeStats();
            }
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
                avgWaiting += currentRunningProcess.waitingTime;
                allocate();
            }
            else
            {
                currentRunningProcess.currentState = RESUMED;
                currentRunningProcess.waitingTime += getClk() - currentRunningProcess.preemptedTime;
            }
            writeStats();
            if (currentRunningProcess.currentState == STARTED)
            {
                int pid = fork();
                if (pid == -1)
                {
                    perror("Error in forking");
                    exit(-1);
                }
                else if (pid == 0)
                {
                    char remainingTime[4];
                    sprintf(remainingTime, "%d", currentRunningProcess.remainingTime);
                    execl("./process.out", "process", remainingTime, (char *)NULL);
                    exit(0);
                }
                sliceStartTime = getClk();
                currentRunningProcess.pid = pid;
            }
            else
            {
                sliceStartTime = getClk();
                kill(currentRunningProcess.pid, SIGCONT);
            }
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
    //  processesCount = 5;
    //  algoNum = 2;
    processesCount = atoi(argv[2]);
    algoNum = atoi(argv[1]);
    policy = atoi(argv[4]);
    WTAarr = (float *)malloc(processesCount * sizeof(float));
    intiallization(); // the array of  WTA
    printf("Num Is=%d \n", processesCount);
    printf("the policy %d", policy);
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
        // timeSlice = 3;
        RR(timeSlice);
        break;
    };
    totalTime = getClk();
    writePerf();
    free(WTAarr);
    destroyClk(false);
    kill(getppid(), SIGINT);
    return 0;
    // clearResources();
}
