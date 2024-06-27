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
char name[1024] = { 0 };
    
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
    SOCKADDR_IN toAddr;
    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(6000);
    toAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    int bOn = 1;
    setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &bOn, sizeof(bOn));
    while (0 == 0)
    {
        sendto(udpSocket, name, strlen(name), 0, (SOCKADDR*)&toAddr, sizeof(toAddr));
        sleep(5);
    }
}

void processTCP()
{
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = inet_addr("172.22.86.63");
    while (0 == 0)
    {
        int tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (connect(tcpSocket, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
        {
            while (0 == 0)
            {
                char buffer[1024] = { 0 };
                int r = recv(tcpSocket, buffer, sizeof(buffer) - 1, 0);
                if (r > 0)
                {
                    while ( buffer[strlen(buffer) - 1] == '\n' || 
                            buffer[strlen(buffer) - 1] == '\n')
                    {
                        buffer[strlen(buffer) - 1] = 0;    
                    }
                    printf("%s\n", buffer);
                }else
                    break;
            }
            close(tcpSocket);
            printf("Clients updated\n");
        }else
            printf("Failed to connect\n");
        sleep(5);
    }
}

int main()
{
    signal(SIGCHLD, sig_child);
    scanf("%s", name);
    if (fork() == 0)
    {
        processUDP();
    }else
    {
        processTCP();
    }
}