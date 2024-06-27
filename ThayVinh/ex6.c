#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
int main()
{
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(SOCKADDR_IN);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(s, 10);
        int c = accept(s, (SOCKADDR*)&caddr, &clen);
        char* welcome = "Hello World, first TCP serve!\n";
        send(c, welcome, strlen(welcome), 0);
        char buffer[1024] = { 0 };
        recv(c, buffer, sizeof(buffer) - 1, 0);
        printf("%s\n", buffer);
        close(c);
    }else
    {
        printf("Failed to bind\n");
    }
    close(s);
}