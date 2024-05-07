#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

typedef struct
{
    int socket;
    char id[20];
    char name[50];
} Client;

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        error("socket() failed");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        error("bind() failed");
    }

    if (listen(listener, 5))
    {
        error("listen() failed");
    }

    fd_set fdread;

    Client clients[MAX_CLIENTS];
    int numClients = 0;
    struct timeval tv;

    int maxdp;
    char buf[BUFFER_SIZE];

    while (1)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        maxdp = listener;

        // Them cac client vao tap tham do
        for (int i = 0; i < numClients; i++)
        {
            FD_SET(clients[i].socket, &fdread);
            if (clients[i].socket > maxdp)
                maxdp = clients[i].socket;
        }

        // Timeout 5s
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int ret = select(maxdp + 1, &fdread, NULL, NULL, &tv);
        if (ret == -1)
        {
            error("select() failed");
        }
        if (ret == 0)
        {
            printf("Timed out.\n");
            continue;
        }

        if (FD_ISSET(listener, &fdread))
        {
            int client = accept(listener, NULL, NULL);
            if (client >= MAX_CLIENTS)
            {
                close(client);
            }
            else
            {
                clients[numClients].socket = client;
                numClients++;
                printf("New client connected: %d\n", client);
            }
        }

        for (int i = 0; i < numClients; i++)
        {
            if (FD_ISSET(clients[i].socket, &fdread))
            {
                int ret = recv(clients[i].socket, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    printf("Client %d disconnected\n", clients[i].socket);
                    close(clients[i].socket);
                    if (i < numClients - 1)
                    {
                        clients[i] = clients[numClients - 1];
                    }
                    numClients--;
                    i--;
                    continue;
                }
                else
                {
                    buf[ret] = '\0';
                    if (strlen(clients[i].id) == 0)
                    {
                        // Chưa nhận tên client, phải kiểm tra xem có đúng định dạng không
                        if (sscanf(buf, "%[^:]: %s", clients[i].id, clients[i].name) != 2)
                        {

                            // Gửi thông báo lỗi đến client và đóng kết nối
                            sprintf(buf, "Invalid format. Please send in the format: client_id: client_name\n");
                            send(clients[i].socket, buf, strlen(buf), 0);
                            close(clients[i].socket);
                            if (i < numClients - 1)
                            {
                                clients[i] = clients[numClients - 1];
                            }
                            numClients--;
                            i--;
                            continue;
                        }
                        else
                        {
                            printf("Client '%s' identified as '%s'\n", clients[i].name, clients[i].id);
                        }
                    }
                    else
                    {
                        // Đã nhận tên client, gửi tin nhắn đến các client khác
                        char timestamp[50];
                        time_t t = time(NULL);
                        struct tm *tm = localtime(&t);
                        strftime(timestamp, sizeof(timestamp), "%Y/%m/%d %I:%M:%S%p", tm);
                        sprintf(buf, "%s %s (%s): %s", timestamp, clients[i].id, clients[i].name, buf);

                        // Gửi tin nhắn đến các client khác
                        for (int j = 0; j < numClients; j++)
                        {
                            if (clients[j].socket != clients[i].socket)
                            {
                                send(clients[j].socket, buf, strlen(buf), 0);
                            }
                        }
                    }
                }
            }
        }
    }

    close(listener);

    return 0;
}
