#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int main(){
    for (int i = 0; i < 5; i++)
    {
        if (fork() == 0)
        {
            // Child process
            int pid = getpid();
            printf("Child process %d started\n", pid);
            while(1){
                sleep(1);
                printf("Child process %d is running\n", pid);
            }
        }
    }
    printf("Parent process\n");
    sleep(5);
    printf("Parent done\n");
    killpg(0, SIGKILL);
    return 0;
}