#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;

int s;
int g_clients[1024];
int g_count = 0;
int pid = 0;

void sig_handler(int signal)
{
    int c = 0;
    printf("Signal received: %d\n", signal);
    FILE* f = fopen("data.txt","rt");
    char buffer[1024] = { 0 };
    fgets(buffer, sizeof(buffer) - 1, f);
    sscanf(buffer, "%d", &c);
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer) - 1, f);
    fclose(f);
    for (int i = 0;i < g_count;i++)
    {
        if (g_clients[i] != c)
        {
            send(g_clients[i], buffer, strlen(buffer), 0);
        }
    }
}

int main()
{
    pid = getpid();
    signal(SIGUSR1, sig_handler);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(s, 10);
    FILE* f = fopen("clients.txt","wt");
    fclose(f);
    while (0 == 0)
    {
        int c = accept(s, (SOCKADDR*)&caddr, &clen);
        printf("A new client accepted\n");
        g_clients[g_count] = c;
        g_count ++;
        if (fork() == 0)
        {
            close(s);
            while (0 == 0)
            {
                char buffer[1024] = { 0 };
                printf("Waiting for data from client\n");
                int r = recv(c, buffer, sizeof(buffer) - 1, 0);
                printf("Received %d bytes from socket %d\n", r, c);
                if (r <= 0)
                {
                    printf("A client has disconnected\n");
                    break;
                }else
                {
                    FILE* f = fopen("data.txt","wt");
                    fprintf(f, "%d\n", c);
                    fprintf(f, "%s", buffer);
                    fclose(f);
                    kill(pid, SIGUSR1);
                }
            }
        }
    }
}