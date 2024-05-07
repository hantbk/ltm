#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

void signalHandler(int signal)
{
    int pid = wait(NULL);
    printf("Child process %d done\n", pid);
}

int main()
{
    signal(SIGCHLD, signalHandler);

    pid_t pid1 = fork();

    if (pid1 == 0)
    {
        printf("Child process 1 start, pid = %d\n", getpid());

        sleep(10);
        printf("Child process 1 running\n");

        exit(0);
    }

    getchar();
    printf("Parent process done\n");

    return 0;
}