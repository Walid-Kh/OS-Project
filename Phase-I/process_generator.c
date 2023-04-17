#include "headers.h"

int clkpid, schpid;
int Qid;

void clearResources(int);

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files. (done)
    char line[50];
    FILE *fp;
    fp = fopen("processes.txt", "r"); // Open processes.txt
    fgets(line, sizeof(line), fp);    // Get first line and ignore it
    struct Queue *q = createQueue();  // call the constractor;

    int z;
    while (!feof(fp)) // Get the data for each process and creat it
    {
        struct process p; // is already defined in Queue.h

        fscanf(fp, "%d", &z); // #id
        p.id = z;
        fscanf(fp, "%d", &z); // arrival
        p.arrival = z;
        fscanf(fp, "%d", &z); // runtime
        p.runtime = z;
        fscanf(fp, "%d", &z); // priority
        p.priority = z;
        fscanf(fp, "%d", &z); // memory size
        p.memsize = z;

        enqueue(q, &p); // there is a error here
    }
    fclose(fp);

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    printf("Which scheduling algorithm do you want?\n");
    printf("1. Non-preemptive Highest Priority First (HPF).\n");
    printf("2. Shortest Remaining time Next (SRTN).\n");
    printf("3. Round Robin (RR).\n");
    printf("======Enter the number of the algorithm you want=====\n");
    int algoNum, timeSlice = 0;
    scanf("%d", &algoNum);
    if (algoNum == 3)
    {
        printf("You choice Round Robin (RR) algorithm ,please enter time slice in second unit");
        while (timeSlice <= 0)
            scanf("%d", &timeSlice);
    }
    printf("======Which allocation policies do you want?=====\n");
    printf("1. First Fit.\n");
    printf("2. Buddy System.\n");
    printf("======Enter the policy number=====\n");
    int policy;
    scanf("%d", &policy);


    //=======================send the selected algorithm to scheduler.c==================
    int send_val;

    Qid = msgget(PG_SH_KEY, 0666 | IPC_CREAT); // Creat the Message Queue

    if (Qid == -1)
    {
        perror("Error in create the Queue");
        exit(-1);
    }

    // 3. Initiate and create the scheduler and clock processes.
    clkpid = fork();
    if (clkpid == 0)
    {
        execl("./clk.out", "./build/clk.out", NULL);
        exit(-1);
    }
/*
    schpid = fork();
    if (schpid == 0)
    {
        char AlgoNumStr[2];
        char processesCountStr[5];
        char timeSliceStr[3];
        char policyStr[2];

        sprintf(AlgoNumStr, "%d", algoNum);
        sprintf(processesCountStr, "%d", q->count);
        sprintf(timeSliceStr, "%d", timeSlice);
        sprintf(policyStr,"%d",policy);

        execl("./scheduler.out", "./scheduler.out", AlgoNumStr, processesCountStr, timeSliceStr,policyStr,(const char *)0);
        exit(0);
    }
    */
    initClk();

    // 4. Use this function after creating the clock process to initialize clock
    // To get time use this

    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters. (done in the above part)
    // 6. Send the information to the scheduler at the appropriate time.
    while (!isQueueEmpty(q))
    {
        while (!isQueueEmpty(q) && q->Front->process.arrival <= getClk())
        {
            struct process temp;
            temp = dequeue(q);
            // =====> send it to scheduler <=======//
            struct processMsg p;
            p.mtype = 1;
            p.process = temp;
            send_val = msgsnd(Qid, &p, sizeof(p.process), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Error in send");
        }
    }
    while (1)
    {
    }
    // 7. Clear clock resources
    // TODO: change later causes seg fault
    // destroyClk(true);
    // clearResources(9);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    printf("\n cleaning resources");
    msgctl(Qid, IPC_RMID, NULL);
    kill(clkpid, SIGINT);
    kill(schpid, SIGINT);
    waitpid(schpid, NULL, 0);
    waitpid(clkpid, NULL, 0);
    exit(0);
}
