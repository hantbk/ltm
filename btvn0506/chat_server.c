#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *client_proc(void *);

int clients[64];

int numClients = 0;
unsigned long threadID[3];

int main()
{
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    while (1)
    {
        if (numClients < 2)
        {
            printf("Waiting for new client\n");
            int client = accept(listener, NULL, NULL);
            printf("New client accepted, client = %d\n", client);
            numClients++;
            clients[numClients] = client;

            pthread_t tid;
            pthread_create(&tid, NULL, client_proc, &client);
            threadID[numClients] = tid;
            pthread_detach(tid);
        }
    }

    return 0;
}

void *client_proc(void *arg)
{
    int client = *(int *)arg;
    char buf[256];
    char msg[512];

    // Nhan du lieu tu client
    while (1)
    {
        if (numClients == 2)
        {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
            {
                numClients = 0;
                for (int i = 1; i <= 2; i++)
                {
                    if (clients[i] != client)
                    {
                        send(clients[i], "Client disconnected", 19, 0);
                        pthread_cancel(threadID[i]);
                    }
                }
                close(client);
                break;
            }

            buf[ret] = 0;

            sprintf(msg, "Received from client %d: %s\n", client, buf);
            printf("Received from %d: %s\n", client, buf);

            for (int i = 1; i <= numClients; i++)
            {
                if (clients[i] != client)
                {
                    send(clients[i], msg, strlen(msg), 0);
                }
            }
        }
    }

    close(client);
}