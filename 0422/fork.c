#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Before fork, pid = %d\n", getpid());
    for(int i = 0; i < 5; i++) {
        if (fork() == 0)
        {
            printf("Child process %d\n", i);
            exit(0);
        }
        
    }
    printf("Done\n");

    return 0;
}