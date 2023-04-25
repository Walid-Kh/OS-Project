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
    int memsize;       // the memory size of the process
    int memstart;      // Start Of The Process In The Memory
    int startingTime;  // starting time given by the algorithm initially = -1
    int runningTime;   // running time as it exist in processes.txt
    int remainingTime; // decremented by algorithm when it's worked on initially = runningTime
    int waitingTime;   // Time
    int finishTime;
    int turnAroundTime;
    int preemptedTime; //Last Time The Process Was Preempted 
    state currentState; // started finished resumed stopped
} PCB;
typedef struct process
{
    int id;
    int arrival;
    int runtime;
    int priority;
    int memsize;
} process;
typedef struct processMsg // message structure of process
{
    long mtype;
    struct process process;
} processMes;
typedef struct Map // message structure of process
{
    int start;
    int end;
    bool ocuupied;
} Map;
#endif