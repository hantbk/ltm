#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
int N, K, Sum = 0;
pthread_mutex_t* mutex = NULL;

struct thread_param
{
    int start;
    int end;
};

void* threadfunc(void* arg)
{
    struct thread_param* param = (struct thread_param*)arg;
    int start = param->start;
    int end = param->end;
    free(param);
    for (int i = start;i <= end;i++)
    {
        pthread_mutex_lock(mutex);
        Sum += i;
        pthread_mutex_unlock(mutex);
    }
    return NULL;
}

int main()
{
    mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    printf("N="); scanf("%d", &N);
    printf("K="); scanf("%d", &K);
    pthread_t* ptid = (pthread_t*)calloc(N, sizeof(pthread_t));

    for (int i = 0;i < N;i++)
    {
        pthread_t tid;
        struct thread_param* arg = (struct thread_param*)calloc(1, sizeof(struct thread_param));
        arg->start = i * (K / N) + 1;
        arg->end = (i + 1) * (K / N);
        if (arg->end > K)
        {
            arg->end = K - 1;
        }
        pthread_create(&tid, NULL, threadfunc, (void*)arg);
        ptid[i] = tid;
    }
    
    for (int i = 0;i < N;i++)
    {
        int* status = NULL;
        pthread_join(ptid[i], (void**)&status);
    }
    free(ptid);
    ptid = NULL;

    printf("%d\n", Sum);
    pthread_mutex_destroy(mutex);
    free(mutex);
}