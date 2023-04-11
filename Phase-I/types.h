#ifndef _TYPES_H
#define _TYPES_H
typedef enum state
{
    STARTED,
    FINISHED,
    RESUMED,
    STOPPED
} state;

typedef struct PCB
{
    int pid;           // pid given by the fork
    int id;            // id in the processes.txt
    int priority;      // priority in processes.txt
    int arrivalTime;   // arrival time as it exist in processes.txt
    int startingTime;  // starting time given by the algorithm initially = -1
    int runningTime;   // running time as it exist in processes.txt
    int remainingTime; // decremented by algorithm when it's worked on initially = runningTime
    int waitingTime;   // Time
    int finishTime;
    int turnAroundTime; 
    state currentState; // started finished resumed stopped
} PCB;
typedef struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
} process;
typedef struct processMsg // message structure of process
{
    long mtype;
    struct process process;
} processMes;

#endif