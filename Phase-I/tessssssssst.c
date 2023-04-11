#include "headers.h"


void  func()
{

    int Qid =msgget(PG_SH_KEY, 0666|IPC_CREAT);

    //while (1) {
        struct processMsg p;
        if (msgrcv(Qid, &p, sizeof(p.process), 0, !IPC_NOWAIT) == -1) {
            perror("error in receiving");
        }

        int pid = fork();

        if (pid == 0) {
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
func();


    return 0;
}