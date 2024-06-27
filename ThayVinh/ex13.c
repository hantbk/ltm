#include <sys/types.h>
#include <sys/wait.h>
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

void sig_child(int signal)
{
    pid_t pid; 
	int stat; 
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) 
    {
		printf("child %d terminated\n", pid); 
    }
}

void processUDP()
{
    int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = 0;
    bind(udpSocket, (SOCKADDR*)&saddr, sizeof(saddr));
    while (0 == 0)
    {
        char buffer[1024] = { 0 };
        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        int r = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&caddr, &clen);
        if (r > 0)
        {
            while ( buffer[strlen(buffer) - 1] == '\r' ||
                    buffer[strlen(buffer) - 1] == '\n')
            {
                buffer[strlen(buffer) - 1] = 0;
            }
            FILE* f = fopen("clients.txt","a+t");
            fprintf(f, "%s %s\n", buffer, inet_ntoa(caddr.sin_addr));
            fclose(f);
            printf("%s %s\n", buffer, inet_ntoa(caddr.sin_addr));
        }
    }
}

void processTCP()
{
    int tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    bind(tcpSocket, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(tcpSocket, 10);
    while (0 == 0)
    {
        SOCKADDR_IN caddr;
        int clen = sizeof(caddr);
        int c = accept(tcpSocket, (SOCKADDR*)&caddr, &clen);
        printf("A new TCP client has connected\n");
        FILE* f = fopen("clients.txt","rt");
        while (!feof(f))
        {
            char buffer[1024] = { 0 };
            fgets(buffer, sizeof(buffer) - 1, f);
            if (strlen(buffer) > 0)
            {
                send(c, buffer, strlen(buffer), 0);
            }
        }
        fclose(f);
        close(c);
    }
}

int main()
{
    FILE* f = fopen("clients.txt","wt");
    fclose(f);
    signal(SIGCHLD, sig_child);
    if (fork() == 0)
    {
        processUDP();
    }else
    {
        processTCP();
    }
}