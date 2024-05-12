#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <ctype.h>

void decode_str1(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (isalpha(str[i])) { // Kiểm tra xem ký tự có phải là chữ cái hay không
            if (str[i] == 'z')
                str[i] = 'a';
            else if (str[i] == 'Z')
                str[i] = 'A';
            else
                str[i] = str[i] + 1; // Thay ký tự chữ cái bằng chữ cái tiếp theo
        } else if (isdigit(str[i])) { // Kiểm tra xem ký tự có phải là chữ số hay không
            str[i] = '9' - str[i] + '0'; // Thay ký tự số bằng chữ số bù 9
        }
        // Tiếp tục với ký tự khác giữ nguyên
        i++;
    }
}

void decode_str(char *str){
    char decoded_str[256];
    int i = 0, j = 0;
    while (str[i] != '\0')
    {
        if((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))
        {
            decoded_str[j] = str[i] + 1;
        } else if(str[i] == 'z')
        {
            decoded_str[j] = 'a';
        } else if(str[i] == 'Z')
        {
            decoded_str[j] = 'A';
        } else if(str[i] >= '0' && str[i] <= '9')
        {
            decoded_str[j] = '9' - (str[i] - '0');
            if(decoded_str[j] < '0') 
                decoded_str[j] += 10;
        }

        if(str[i] == '\0')
            break;

        // Copy ky tu dau cach
        if(str[i] == ' ')
            decoded_str[j] = str[i];

        i++;
        j++;
    }
    
    //Kiem tra ky tu cuoi cung cua chuoi co phai la dau cach hay khong
    if(decoded_str[j - 1] == ' ')
        decoded_str[j - 1] = '\0';
    else
        decoded_str[j] = '\0';
    strcpy(str, decoded_str);
}

int main() 
{
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) 
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) 
    {
        perror("listen() failed");
        return 1;
    }

    struct pollfd fds[64];
    int nfds = 1;

    fds[0].fd = listener;
    fds[0].events = POLLIN;

    char buf[256];

    while (1)
    {
        int ret = poll(fds, nfds, -1);
        if (ret < 0)
        {
            perror("poll() failed");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            int client = accept(listener, NULL, NULL);
            if (nfds == 64)
            {
                // Tu choi ket noi
                close(client);
            }
            else
            {
                fds[nfds].fd = client;
                fds[nfds].events = POLLIN;

                char msg[100];
                sprintf(msg, "Dang co %d clients dang ket noi.\n", nfds);
                send(fds[nfds].fd, msg , strlen(msg), 0);

                nfds++;

                printf("New client connected: %d\n", client);
            }
        }

        for (int i = 1; i < nfds; i++)
            if (fds[i].revents & POLLIN)
            {
                ret = recv(fds[i].fd, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    close(fds[i].fd);
                    // Xoa khoi mang
                    if (i < nfds - 1)
                        fds[i] = fds[nfds - 1];
                    nfds--;
                    i--;
                }
                else
                {
                    if(strncmp(buf, "exit", 4) == 0)
                    {
                        char msg[100];
                        sprintf(msg, "Goodbye!");
                        send(fds[i].fd, msg, strlen(msg), 0);
                        close(fds[i].fd);
                        // Xoa khoi mang
                        if (i < nfds - 1)
                            fds[i] = fds[nfds - 1];
                        nfds--;
                        i--;
                        continue;
                    }

                    buf[ret] = 0;
                    printf("Received from %d: %s\n", fds[i].fd, buf);
                    buf[ret] = 0;
                    decode_str1(buf);
                    send(fds[i].fd, buf, strlen(buf), 0);
                }
            }
    }
    
    close(listener);    

    return 0;
}