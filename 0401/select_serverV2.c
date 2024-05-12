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

    // Khai báo tập fdread chứa các socket và tập fdtest để thăm dò sự kiện
    fd_set fdread, fdtest;
    FD_ZERO(&fdread);

    // Gan cac socket vao tap fread
    FD_SET(listener, &fdread);

    char buf[256];
    struct timeval tv;

    while (1)
    {
        // Giữ nguyên các socket trong tập fdread
        fdtest = fdread;

        // Khởi tạo cấu trúc thời gian
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        // Gọi hàm select() chờ đến khi sự kiện xảy ra hoặc hết giờ
        int ret = select(FD_SETSIZE, &fdread, NULL, NULL, &tv);
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

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            // Kiem tra su kien cua socket nao
            if (FD_ISSET(i, &fdtest))
            {
                // Socket listener có sự kiện yêu cầu kết nối
                if (i == listener)
                {
                    // Co ket noi moi
                    int client = accept(listener, NULL, NULL);
                    if (client >= FD_SETSIZE) // Đã vượt quá số kết nối tối đa
                    {
                        close(client);
                    }
                    else // Chưa vượt quá số kết nối tối đa
                    {
                        // Thêm socket vào tập sự kiện
                        FD_SET(client, &fdread);
                        printf("New client connected: %d\n", client);
                    }
                }
                else
                {
                    // Socket client có sự kiện nhận dữ liệu
                    int client = i;
                    int ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        close(client);
                        FD_CLR(client, &fdread); // Xoá socket ra khỏi tập sự kiện
                        printf("Client %d disconnected\n", client);
                    }
                    else
                    {
                        buf[ret] = 0;
                        printf("Received data from %d: %s\n", client, buf);
                    }
                }
            }
        }
    }

    close(listener);

    return 1;
}