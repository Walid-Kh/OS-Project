#include "headers.h"

void func()
{

    // int Qid =msgget(PG_SH_KEY, 0666|IPC_CREAT);

    // while (1) {

    int pid = fork();

    if (pid == 0)
    {
        printf("child");
        execl("./process.out", "./process.out", 50, NULL);
        exit(0);
    }
    printf("parent");
    //}
}
int main()
{
    func();
    return 0;
}