#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int main() {
    int cid = fork();
    if(cid == 0) {
        // Child process
        printf("Child proceess started\n");
        while (1)
        {
            sleep(1);
            printf("Child process is running\n");
        }
        exit(EXIT_SUCCESS);
    }
    printf("Parent process \n");
    sleep(5);
    kill(cid, SIGKILL);
    printf("Parent done\n");
    return 0;
}