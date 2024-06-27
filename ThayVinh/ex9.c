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
int g_count = 0;
SOCKADDR_IN* g_addr = NULL;

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
            int found = 0;
            for (int i = 0;i < g_count;i++)
            {
                if (g_addr[i].sin_addr.s_addr == caddr.sin_addr.s_addr)
                {
                    found = 1;
                    break;
                }
            }
            if (found == 0)
            {
                g_addr = (SOCKADDR_IN*)realloc(g_addr, (g_count + 1) * sizeof(SOCKADDR_IN));
                g_addr[g_count] = caddr;
                g_count += 1;
            }
            printf("Received %d bytes from %s\n", r, inet_ntoa(caddr.sin_addr));
            for (int i = 0;i < g_count;i++)
            {
                if (g_addr[i].sin_addr.s_addr != caddr.sin_addr.s_addr)
                {
                    sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&g_addr[i], sizeof(SOCKADDR_IN));
                }
            }
        }
    }else
        printf("Failed to bind\n");

    close(s);
}