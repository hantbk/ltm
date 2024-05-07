#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

int remove_client(int client, int *client_socket, char **name_clients, int *num_clients)
{
    // Xoa client khoi danh sach client da dang nhap
    int i = 0;
    for (; i < *num_clients; i++)
    {
        if (client_socket[i] == client)
        {
            break;
        }
    }
    if (i < *num_clients)
    {
        if (i < num_clients - 1)
        {
            client_socket[i] = client_socket[*num_clients - 1];
            strcpy(name_clients[i], name_clients[*num_clients - 1]);
        }
        free(name_clients[*num_clients - 1]);
        (*num_clients)--;
    }
}

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

    int client_sockets[FD_SETSIZE];
    char *name_clients[FD_SETSIZE];
    int num_clients = 0;

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

                        // Xoa client khoi danh sach client da dang nhap
                        remove_client(client, client_sockets, name_clients, &num_clients);
                        continue;
                    }

                    // In ra man hinh server
                    buf[ret] = 0;
                    printf("Received data from %d: %s\n", client, buf);

                    // Kiem tra trang thai dang nhap
                    int j = 0;
                    for (; j < num_clients; j++)
                    {
                        if (client_sockets[j] == client)
                        {
                            break;
                        }
                        if (j == num_clients)
                        {
                            // Chua dang nhap
                            char cmd[16], id[32], tmp[32];
                            int n = sscanf(buf, "%s %s %s", cmd, id, tmp);
                            if (n == 2 && strcmp(cmd, "client_id:") == 0)
                            {
                                // Kiem tra id co trung voi id cua client khac da dang nhap khong
                                int k = 0;
                                for (; k < num_clients; k++)
                                    if (strcmp(name_clients[k], id) == 0)
                                        break;
                                if (k < num_clients)
                                {
                                    // Id da ton tai
                                    char *msg = "Id da ton tai. Hay nhap lai.\n";
                                    send(client, msg, strlen(msg), 0);
                                }
                                else
                                {
                                    // Id chua ton tai
                                    char *msg = "Dang nhap thanh cong.\n";
                                    send(client, msg, strlen(msg), 0);

                                    // Chuyen client sang trang thai da dang nhap
                                    client_sockets[num_clients] = client;
                                    name_clients[num_clients] = malloc(strlen(id) + 1);
                                    memcpy(name_clients[num_clients], id, strlen(id) + 1);
                                    // strcpy(name_clients[num_clients], id);
                                    // name_clients[num_clients] = strdup(id);
                                    num_clients++;
                                }
                            }
                            else
                            {
                                char *msg = "Sai cu phap. Hay nhap lai.\n";
                                send(client, msg, strlen(msg), 0);
                            }
                        }
                        else
                        {
                            // Da dang nhap

                            char receiver[32];
                            sscanf(buf, "%s", receiver);

                            if (strcmp(receiver, "all") == 0)
                            {
                                // Chuyen tiep tin nhan cho cac client khac da dang nhap
                                for (int k = 0; k < num_clients; k++)
                                {
                                    if (client_sockets[k] != client)
                                    {
                                        char msg[256];
                                        sprintf(msg, "%s: %s\n", name_clients[j], buf + strlen(receiver) + 1);
                                        send(client_sockets[k], msg, strlen(msg), 0);
                                    }
                                }
                            }
                            else
                            {
                                // Chuyen tiep tin nhan cho client co ten receiver
                                int k = 0;
                                for (; k < num_clients; k++)
                                {
                                    if (strcmp(name_clients[k], receiver) == 0)
                                    {
                                        char msg[256];
                                        sprintf(msg, "%s: %s\n", name_clients[j], buf + strlen(receiver) + 1);
                                        send(client_sockets[k], msg, strlen(msg), 0);
                                        break;
                                    }
                                }
                                if (k == num_clients)
                                {
                                    char *msg = "Khong tim thay nguoi nhan.\n";
                                    send(client, msg, strlen(msg), 0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    close(listener);

    return 1;
}