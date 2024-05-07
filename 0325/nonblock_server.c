#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

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
    addr.sin_port = htons(9000);

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

    char buf[1024];

    int clients[64];
    int numClients = 0;

    while (1)
    {
        // printf("Waiting for client...\n");
        int client = accept(listener, NULL, NULL);
        if (client != -1)
        {
            printf("Client %d connected\n", client);
            unsigned long ul = 1;
            ioctl(client, FIONBIO, &ul);

            clients[numClients] = client;
            numClients++;
        }
        else
        {
            perror("accept() failed");
            break;
        }
        

        for (int i = 0; i < numClients; i++)
        {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret != -1)
            {
                if (ret <= 0)
                {
                    continue;
                }

                buf[ret] = 0;
                printf("Received from Client %d: %s\n", clients[i], buf);
            }
            else
            {
                if (errno == EWOULDBLOCK)
                {
                    // Khong xu ly gi them
                    // printf("Khong co du lieu\n");
                }
                else
                {
                    // perror("recv() failed");
                    continue;
                }
            }
        }
    }

    close(listener);

    return 0;
}