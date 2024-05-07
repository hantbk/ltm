#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

#define FD_SETSIZE 256
#define MAX_CLIENTS 10

typedef struct
{
    int socket;
    char username[20];
    char password[20];
} Client;

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

    Client clients[FD_SETSIZE];
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
            perror("select() failed");
            continue;
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
            // Neu co su kien tu client i
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
                    if (strcmp(buf, "login") == 0)
                    {
                        printf("Send me username + password\n");
                        char user[20], pass[20];
                        recv(clients[i].socket, user, sizeof(user), 0);
                        recv(clients[i].socket, pass, sizeof(pass), 0);

                        FILE *f = fopen("accounts.txt", "r");
                        if (f == NULL)
                        {
                            perror("Cannot open file");
                            close(clients[i].socket);
                            continue;
                        }

                        int loggedIn = 0;
                        char line[256];
                        while (fgets(line, sizeof(line), f))
                        {
                            line[strlen(line) - 1] = '\0';
                            char *u = strtok(line, " ");
                            char *p = strtok(NULL, " ");
                            if (strcmp(user, u) == 0 && strcmp(pass, p) == 0)
                            {
                                loggedIn = 1;
                                send(clients[i].socket, "Dang nhap thanh cong", 20, 0);
                                break;
                            }
                        }

                        fclose(f);

                        if (!loggedIn)
                        {
                            send(clients[i].socket, "Dang nhap that bai", 20, 0);
                            close(clients[i].socket); // Đóng kết nối nếu đăng nhập không thành công
                            if (i < numClients - 1)
                            {
                                clients[i] = clients[numClients - 1];
                            }
                            numClients--;
                            i--;
                        }
                    }
                    else if (strcmp(buf, "execute") == 0)
                    {
                        printf("Send me command\n");
                        char cmd[256];
                        recv(clients[i].socket, cmd, sizeof(cmd), 0);

                        // Kiểm tra xem đã đăng nhập chưa trước khi thực hiện lệnh
                        if (strlen(clients[i].username) == 0 || strlen(clients[i].password) == 0)
                        {
                            send(clients[i].socket, "Chua dang nhap", 20, 0);
                        }
                        else
                        {
                            // Thực thi lệnh và gửi kết quả trở lại client
                            char cmd_with_redirect[256];
                            snprintf(cmd_with_redirect, sizeof(cmd_with_redirect), "%s > out.txt", cmd);
                            system(cmd_with_redirect);

                            // Đọc kết quả từ file out.txt và gửi lại cho client
                            FILE *output = fopen("out.txt", "r");
                            if (output == NULL)
                            {
                                send(clients[i].socket, "Loi", 20, 0);
                            }
                            else
                            {
                                char result[256];
                                while (fgets(result, sizeof(result), output) != NULL)
                                {
                                    send(clients[i].socket, result, strlen(result), 0);
                                }
                                fclose(output);
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
