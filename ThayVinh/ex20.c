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
int* g_clients = NULL;
int g_count = 0;
fd_set rset;
int main()
{
    SOCKADDR_IN saddr,caddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(s, 10);
        while (0 == 0)
        {
            int clen = sizeof(caddr);
            FD_ZERO(&rset);
            FD_SET(s, &rset);
            for (int i = 0;i < g_count;i++)
            {
                FD_SET(g_clients[i], &rset);
            }

            int r = select(FD_SETSIZE, &rset, NULL, NULL, NULL);

            if (r > 0)
            {
                if (FD_ISSET(s, &rset))
                {
                    int c = accept(s, (SOCKADDR*)&caddr, &clen);
                    g_clients = (int*)realloc(g_clients, (g_count + 1) * sizeof(int));
                    g_clients[g_count++] = c;
                    char* welcome = "Hello IP Multiplexing\n";
                    send(c, welcome, strlen(welcome), 0);
                }
                for (int i = 0;i < g_count;i++)
                {
                    if (FD_ISSET(g_clients[i], &rset))
                    {
                        char buffer[1024] = { 0 };
                        int r = recv(g_clients[i], buffer, sizeof(buffer) - 1, 0);
                        if (r > 0)
                        {
                            for (int j = 0;j < g_count;j++)
                            {
                                if (j != i)
                                {
                                    send(g_clients[j], buffer, strlen(buffer), 0);
                                }        
                            }
                        }
                    }
                }
            }
        }
    }else
        printf("Failed to bind\n");

    close(s);
}