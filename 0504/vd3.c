// Xung dot giua cac luong

#include <stdio.h>
#include <pthread.h>
#define NLOOP 100
int counter; // Tài nguyên găng
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Khoá mutex 

void *doit(void *);
int main()
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, doit, NULL);
    pthread_create(&t2, NULL, doit, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
void *doit(void *args)
{
    for (int i = 0; i < NLOOP; i++)
    {
        pthread_mutex_lock(&counter_mutex); // Thêm khoá mutex
        int val = counter;
        printf("%ld: %d\n", pthread_self(), val + 1);
        counter = val + 1;
        pthread_mutex_unlock(&counter_mutex); // Mở khoá cho phép truy nhập tài nguyên găng
    }
}
