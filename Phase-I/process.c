#include "headers.h"
/* Modify this file as needed*/
int remainingtime;
// void handler(int signnum);
int main(int agrc, char *argv[])
{
    // signal(SIGINT, handler);
    initClk();
    remainingtime = atoi(argv[1]);
    printf("Iam Alive");
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
    printf("Ok");
    kill(getppid(), SIGUSR2);

    destroyClk(false);

    return 0;
}
// For Preemptive Algorihtms   By Manga
/*void handler(int signnum)
{



}
*/