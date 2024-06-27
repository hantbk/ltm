#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
    char serverip[1024] =  { 0 };
    char filename[512] = { 0 };
    int port = 0;
    printf("Server IP: ");
    scanf("%s", serverip);
    printf("Filename: ");
    scanf("%s",filename);
    printf("Port: ");
    scanf("%d", &port);
    char buffer[1024] = { 0 };
    sprintf(buffer, "GET %s %d", filename, port);
    int c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = inet_addr(serverip);
    if (connect(c, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        send(c, buffer, strlen(buffer), 0);
        sleep(1);
        int d = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        saddr.sin_port = htons(port);
        if (connect(d, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
        {
            while (0 == 0)
            {
                memset(buffer, 0, sizeof(buffer));
                int r = recv(d, buffer, sizeof(buffer) - 1, 0);
                if (r > 0)
                {
                    printf("%s", buffer);
                }else
                    break;
            }
            close(d);
            memset(buffer, 0, sizeof(buffer));
            int r = recv(c, buffer, sizeof(buffer) - 1, 0);
            if (r > 0)
            {
                printf("%s", buffer);
            }
        }
        close(c);
    }
}