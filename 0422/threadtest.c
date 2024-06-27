#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *thread_proc(void *param){
    int *n = (int *)param;
    *n = *n + 10;
    return NULL;
}

int main(){
    int n = 10;
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, thread_proc, &n);
    pthread_join(thread_id, NULL);
    printf("n = %d\n", n);
    return 0;
}
