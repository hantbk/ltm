#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t pid1 = fork();
    pid_t pid2 = fork();

    if (pid1 == 0)
    {
        printf("Child process 1 start, pid = %d\n", getpid());
        while (1)
        {
            sleep(1);
            printf("Child process 1 running\n");
        }
        
        exit(0);
    }

    if (pid2 == 0)
    {
        printf("Child process 2 start, pid = %d\n", getpid());
        while (1)
        {
            sleep(1);
            printf("Child process 2 running\n");
        }
        
        exit(0);
    }

    sleep(10);
    killpg(0, SIGKILL);
    
    printf("Parent process done\n");

    return 0;
}