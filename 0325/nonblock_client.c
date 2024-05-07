#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

int main()
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    // Chuyen socket sang che do bat dong bo
    unsigned long ul = 1;
    ioctl(client, FIONBIO, &ul);

    //Chuyen STDIN sang che do bat dong bo
    ioctl(STDIN_FILENO, FIONBIO, &ul);

    char buf[256];
    while (1)
    {
        // Doc du lieu tu ban phim
        // printf("Nhap chuoi ky tu: ");
        if(fgets(buf, sizeof(buf), stdin) != NULL) {
            send(client, buf, strlen(buf), 0);
            if (strncmp(buf, "exit", 4) == 0)
            {
                break;
            }
        }

        // Nhan du lieu tu server
        // printf("Dang cho tu Server: \n");
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret != -1)
        {
            if(ret <= 0)
                break;
            buf[ret] = 0;
            printf("Server: %s\n", buf);
        } else
        {
            if(errno == EWOULDBLOCK) {
                // printf("Khong co du lieu\n");
            }
            else {
                perror("recv() failed");
                break;
            }
        }
        
    }

    close(client);

    return 0;
}