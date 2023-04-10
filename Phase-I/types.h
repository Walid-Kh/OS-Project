#ifndef __TYPES_H__
#define __TYPES_H__
typedef enum state
{
    STARTED,
    FINISHED,
    RESUMED,
    STOPPED
} state;

typedef struct PCB
{
    int pid;            // pid given by the fork
    int id;             // id in the processes.txt
    int priority;       // priority in processes.txt
    int arrivalTime;    // arrival time as it exist in processes.txt
    int startingTime;   // starting time given by the algorithm initially = -1
    int runningTime;    // running time as it exist in processes.txt
    int remainingTime;  // decremented by algorithm when it's worked on initially = runningTime
    state currentState; // started finished resumed stopped
} PCB;

#endif