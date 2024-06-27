#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
pthread_mutex_t* mutex = NULL;

int RECV(int c, char* buffer, int len)
{
    int received = 0;
    while (0 == 0)
    {
        int tmp = recv(c, buffer, len - received, 0);
        if (tmp > 0)
        {
            received += tmp;
            if (received == len)
            {
                break;
            }
            if (tmp < len - received)
            {
                break;
            }
        }else
        {
            break;
        }
    }
    return received;
}

void* ClientThread(void* arg)
{
    int c = *((int*)arg);
    free(arg);
    arg = NULL;
    char buffer[1024] = { 0 };
    while (0 == 0)
    {
        memset(buffer, 0, sizeof(buffer));
        int r = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (r > 0)
        {
            while (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
            {
                buffer[strlen(buffer) - 1] = 0;
            }
            if (strcmp(buffer, "exit") == 0)
            {
                break;
            }else
            {
                sprintf(buffer + strlen(buffer), "> tmp.txt");
                pthread_mutex_lock(mutex);
                system(buffer);
                pthread_mutex_unlock(mutex);

                FILE* f = fopen("tmp.txt","rt");
                while (!feof(f))
                {
                    memset(buffer, 0, sizeof(buffer));
                    fgets(buffer, sizeof(buffer) - 1, f);
                    send(c, buffer, strlen(buffer), 0);
                }
                fclose(f);
            }
        }else
        {
            printf("A client has disconnected\n");
            break;
        }
    }
    close(c);
}

int main()
{
    mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = 0;
    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(s, 10);
        while (0 == 0)
        {
            int c = accept(s, (SOCKADDR*)&caddr, &clen);
            int* arg = (int*)calloc(1,sizeof(int));
            *arg = c;
            pthread_t tid;
            pthread_create(&tid, NULL, ClientThread, (void*)arg);
        }
    }else
    {
        printf("Failed to bind!\n");
    }
    close(s);

    pthread_mutex_destroy(mutex);
    free(mutex);
}