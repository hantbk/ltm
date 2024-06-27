#include<stdio.h>
#include<unistd.h>

int main(){
    int n = 1;
    if(fork())
        n = n + 1;
    else
        n = n + 2;
    printf("n = %d\n", n);
    return 0;
}