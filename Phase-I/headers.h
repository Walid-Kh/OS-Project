#ifndef _HEADERS_H
#define _HEADERS_H

#include <stdio.h> //if you don't use scanf/printf change this include
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "types.h"
#include "Queue.h"

#define SHKEY 300
#define PG_SH_KEY 90

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

int getClk()
{
    if (shmaddr)
        return *shmaddr;
    else
        return 0;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT); // TODO: Change this later
    }
}
char *stateToString(enum state s)
{
    switch (s)
    {
    case STARTED:
        return "started";
        break;
    case STOPPED:
        return "stopped";
        break;
    case FINISHED:
        return "finished";
        break;
    case RESUMED:
        return "resumed";
        break;

    default:
        return "I LOVE OS";
        break;
    }
}

#endif