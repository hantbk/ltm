#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int main()
{
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr, caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);
    saddr.sin_addr.s_addr = 0;
    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        while (0 == 0)
        {
            char buffer[1024] = { 0 };
            int clen = sizeof(caddr);
            int r = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, &clen);
            printf("Received %d bytes from %s\n", r, inet_ntoa(caddr.sin_addr));
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&caddr, sizeof(caddr));
        }
    }else
        printf("Failed to bind\n");

    close(s);
}