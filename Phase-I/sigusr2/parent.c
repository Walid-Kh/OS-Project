#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void handler(int signum)
{
    if (signum == SIGUSR2)
    {
        for (int i = 0; i < 10; i++)
        {
            printf("USR2\n");
        }
    }
    signal(SIGUSR2, handler);
}
int main()
{
    signal(SIGUSR2, handler);
    int pid = fork();
    if (pid == 0)
    {
        execl("child.out", "child.out", NULL);
    }
    while (1)
    {
        sleep(1);
        printf("main\n");
        fflush(stdout);
    }
}