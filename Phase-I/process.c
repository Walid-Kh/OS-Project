#define _XOPEN_SOURCE 700
#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int lastClk;

void handler(int signum)
{
    lastClk = getClk();
    signal(SIGCONT, handler);
}

int main(int argc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[1]);

    struct sigaction act;
    act.sa_handler = handler;

    sigaction(SIGCONT, &act, NULL);

    // TODO it needs to get the remaining time from somewhere
    // remainingtime = ??;
    lastClk = getClk();
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
