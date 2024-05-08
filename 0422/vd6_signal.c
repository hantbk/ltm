#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

void signalHandler(int signal)
{
    int pid = wait(NULL);
    printf("Child process terminated, pid = %d\n", pid);
}

int main()
{
    signal(SIGCHLD, signalHandler);

    pid_t pid = fork();

    if (pid == 0)
    {
        printf("Child process start, pid = %d\n", getpid());

        sleep(10);
        printf("Child process done, pid = %d\n", getpid());

        exit(0);
    }

    getchar();
    printf("Parent process done\n");

    return 0;
}