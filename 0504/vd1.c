#include <stdio.h>
#include <pthread.h>

void *thread_proc(void *);

int main() {
    pthread_t tid;
    int params[8];

    for (int i = 0; i < 8; i++)
    {
        params[i] = i;
        pthread_create(&tid, NULL, thread_proc, &params[i]);
        printf("New thread created tid = %lu, address = %p\n", tid, &params[i]);
    }
    getchar();
}

void *thread_proc(void *args){
    /* CHILD THREAD JOB*/
    int param = *(int *)args;
    printf("Child Thread id = %lu, param = %d\n", pthread_self(), param);
}