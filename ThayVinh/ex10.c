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
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

    SOCKADDR_IN saddr, caddr, baddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);
    saddr.sin_addr.s_addr = 0;

    baddr.sin_family = AF_INET;
    baddr.sin_port = htons(9000);
    baddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        while (0 == 0)
        {
            char buffer[1024] = { 0 };
            int clen = sizeof(caddr);
            int r = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, &clen);
            printf("Received %d bytes from %s\n", r, inet_ntoa(caddr.sin_addr));
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&baddr, sizeof(SOCKADDR_IN));
        }
    }else
        printf("Failed to bind\n");

    close(s);
}