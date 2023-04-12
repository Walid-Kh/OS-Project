#include <signal.h>
#include <unistd.h>
int main()
{
    sleep(1000);
    kill(getppid(), SIGUSR2);
}