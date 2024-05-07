#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define PORT 9000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Hàm mã hóa xâu ký tự
void encode_message(char *message)
{
    while (*message != '\0')
    {
        if ((*message >= 'a' && *message < 'z') || (*message >= 'A' && *message < 'Z'))
            *message += 1;
        else if (*message == 'z')
            *message = 'a';
        else if (*message == 'Z')
            *message = 'A';
        else if (*message >= '0' && *message < '9')
            *message = '9' - (*message - '0');
        message++;
    }
}

int main()
{
    int server_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    fd_set read_fds;
    int max_sd, activity, i, valread, client_count = 0;
    char buffer[BUFFER_SIZE];
    socklen_t client_addr_len;

    // Tạo server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập thông tin của server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind server socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(server_socket, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Chấp nhận kết nối đến các client
    for (i = 0; i < MAX_CLIENTS; ++i)
        client_sockets[i] = 0;

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        max_sd = server_socket;

        for (i = 0; i < MAX_CLIENTS; ++i)
        {
            if (client_sockets[i] > 0)
                FD_SET(client_sockets[i], &read_fds);
            if (client_sockets[i] > max_sd)
                max_sd = client_sockets[i];
        }

        // Sử dụng select để đợi sự kiện có thể đọc
        activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("Select error");
        }

        // Xử lý kết nối đến server
        if (FD_ISSET(server_socket, &read_fds))
        {
            int client_socket;
            client_addr_len = sizeof(client_addr);
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
            {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port is: %d\n", client_socket,
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Gửi xâu chào kèm số lượng client đang kết nối
            sprintf(buffer, "Xin chào. Hiện có %d clients đang kết nối.\n", client_count);
            send(client_socket, buffer, strlen(buffer), 0);

            // Thêm client vào danh sách
            for (i = 0; i < MAX_CLIENTS; ++i)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = client_socket;
                    break;
                }
            }
            client_count++;
        }

        // Xử lý dữ liệu từ các client
        for (i = 0; i < MAX_CLIENTS; ++i)
        {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &read_fds))
            {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0)
                {
                    getpeername(sd, (struct sockaddr *)&client_addr, &client_addr_len);
                    printf("Client disconnected, ip %s, port %d\n", inet_ntoa(client_addr.sin_addr),
                           ntohs(client_addr.sin_port));
                    close(sd);
                    client_sockets[i] = 0;
                    client_count--;
                }
                else
                {
                    buffer[valread] = '\0';
                    // Kiểm tra nếu client muốn thoát
                    if (strcmp(buffer, "exit\n") == 0)
                    {
                        // Gửi xâu chào tạm biệt và đóng kết nối
                        send(sd, "Xin chào và tạm biệt!\n", strlen("Xin chào và tạm biệt!\n"), 0);
                        printf("Client disconnected, socket fd is %d\n", sd);
                        close(sd);
                        client_sockets[i] = 0;
                        client_count--;
                    }
                    else
                    {
                        // Mã hóa tin nhắn
                        encode_message(buffer);
                        // Hiển thị tin nhắn đã được mã hóa trên server
                        printf("Received and encoded message from client: %s\n", buffer);
                        // Gửi tin nhắn đã được mã hóa cho client
                        send(sd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }
    return 0;
}