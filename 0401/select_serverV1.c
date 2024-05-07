#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

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

    fd_set fdread;

    int clients[FD_SETSIZE];
    int numClients = 0;
    struct timeval tv;

    int maxdp;
    char buf[256];

    while (1)
    {
        // Khoi tao tap fdread
        FD_ZERO(&fdread);

        // Gan cac socket vao tap fdread
        FD_SET(listener, &fdread);
        maxdp = listener;

        for (int i = 0; i < numClients; i++)
        {
            FD_SET(clients[i], &fdread);
            if (clients[i] > maxdp)
                maxdp = clients[i];
        }

        //Thiết lập thời gian chờ
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        // Gọi hàm select() chờ đến khi sự kiện xảy ra trong thời gian timeval
        int ret = select(maxdp + 1, &fdread, NULL, NULL, &tv);
        if (ret == -1)
        {
            printf("select() failed.\n");
            break;
        } 
        if (ret == 0)
        {
            printf("Timed out.\n");
            continue;
        }
        
        // Kiểm tra nếu là sự kiện có yêu cầu kết nối
        if (FD_ISSET(listener, &fdread))
        {
            // Có client mới kết nối đến
            int client = accept(listener, NULL, NULL);
            if (client >= FD_SETSIZE)
            {
                close(client);
            }
            else
            {
                // Lưu socket vào mảng
                clients[numClients] = client;
                numClients++;
                printf("New client connected: %d\n", client);
            }            
        }

        // Kiểm tra sự kiện nhận dữ liệu của các socket client
        for (int i = 0; i < numClients; i++)
        {
            if (FD_ISSET(clients[i], &fdread))
            {
                ret = recv(clients[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    // Ket noi bi ngat
                    printf("Client %d disconnected\n", clients[i]);
                    close(clients[i]);

                    // Xoa socket tai vi tri i
                    if (i < numClients - 1)
                    {
                        clients[i] = clients[numClients - 1];
                    }
                    numClients--;
                    i--;

                    continue;
                } else
                {
                    // Xu ly du lieu nhan duoc
                    buf[ret] = 0;
                    printf("Received from %d: %s\n", clients[i], buf);
                }
            } 
        }
    }

    close(listener);

    return 0;
}