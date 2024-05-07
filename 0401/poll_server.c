#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>

#define MAX_FDS 2048 // So luong socket toi da

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

    struct pollfd fds[MAX_FDS];
    int nfds = 0;

    // Gan socket listener vao tap tham do
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    nfds++;

    char buf[256];

    while (1)
    {
        int ret = poll(fds, nfds, -1);
        if (ret == -1)
        {
            break;
        }
        if (ret == 0)
        {
            printf("Time out\n");
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == listener)
                {
                    int client = accept(listener, NULL, NULL);
                    if (client >= MAX_FDS)
                    {
                        // Vượt quá số lượng tối đa
                        close(client);
                    } else
                    {
                        // Thêm vào mảng thăm dò sự kiện
                        fds[nfds].fd = client;
                        fds[nfds].events = POLLIN;
                        nfds++;

                        printf("New client connected: %d\n", client);   
                    }
                } else
                {
                    // Có dữ liệu từ client truyền đến 
                    int client = fds[i].fd;
                    ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        // Ket noi bi dong => Xoa khoi mang
                    } else
                    {
                        // Xử lý dữ liệu nhận được
                        buf[ret] = 0;
                        printf("Received from %d: %s\n", client, buf);
                    }   
                }
            }
        }
    }

    close(listener);

    return 1;
}