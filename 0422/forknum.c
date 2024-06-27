#include<stdio.h>
#include<unistd.h>

int main(){
    printf("Before fork, pid = %d\n", getpid());
    fork();
    fork();
    printf("After fork, pid = %d\n", getpid());
}