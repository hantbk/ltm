#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0)
    {
        printf("Child process start, pid = %d\n", getpid());
        sleep(15);
        printf("Child process done\n");
        exit(0);
    }

    if (fork() == 0)
    {
        printf("Child process start, pid = %d\n", getpid());
        sleep(10);
        printf("Child process done\n");
        exit(0);
    }

    printf("Waiting for child process\n");
    while (wait(NULL) != -1);
    
    printf("Parent process done\n");

    return 0;
}