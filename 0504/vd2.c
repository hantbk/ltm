#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *thread_proc(void *);

int sum = 0;

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, NULL);
    printf("New thread created.\n");
    pthread_join(tid, NULL);
    printf("sum = %d", sum);
}

void *thread_proc(void *args)
{
    sum = 0;
    printf("Start computing\n");
    for (int i = 1; i <= 10; i++)
    {
        sleep(1);
        sum += i * i;
    }
    printf("Done.\n");
}