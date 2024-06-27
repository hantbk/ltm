#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
int g_clients[1024];
int g_count = 0;

void* ClientThread(void* arg)
{
    int c = *((int*)arg);
    free((int*)arg);    
    while (0 == 0)
    {
        char buffer[1024] = { 0 };
        int r = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (r > 0)
        {
            printf("Received: %s from %d\n", buffer, c);
            for (int i = 0;i < g_count;i++)
            {
                if (g_clients[i] != c)
                {
                    send(g_clients[i], buffer, strlen(buffer), 0);
                }
            }
        }else
            break;
    }
    close(c);
}

int main()
{
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(s, 10);
    while (0 == 0)
    {
        int c = accept(s, (SOCKADDR*)&caddr, &clen);
        printf("A new client connected\n");
        g_clients[g_count++] = c;
        pthread_t tid;
        int* arg = (int*)calloc(1, sizeof(int));
        *arg = c;
        pthread_create(&tid, NULL, ClientThread, (void*)arg);
    }
}