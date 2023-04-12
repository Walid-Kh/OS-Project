#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[1]);

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    int lastClk = getClk();
    while (remainingtime > 0)
    {
        int currentTime = getClk();
        if (lastClk < currentTime)
        {
            lastClk = currentTime;
            remainingtime--;
        }
    }
    kill(getppid(), SIGUSR2);

    destroyClk(false);

    return 0;
}
