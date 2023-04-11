#include <signal.h>
#include <unistd.h>
int main()
{
    sleep(10);
    kill(getppid(), SIGUSR2);
}