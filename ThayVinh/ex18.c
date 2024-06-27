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

void* ClientThread(void* arg)
{
    int c = *((int*)arg);
    free(arg);
    arg = NULL;
    while (0 == 0)
    {
        char buffer[1024] = { 0 };
        int r = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (r > 0)
        {
            char command[1024] = { 0};
            char filename[1024] = { 0 };
            int port = 0;
            sscanf(buffer, "%s%s%d", command, filename, &port);
            if (strcmp(command, "GET") == 0)
            {
                int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                SOCKADDR_IN saddr, caddr;
                int clen = sizeof(caddr);
                saddr.sin_family = AF_INET;
                saddr.sin_port = htons(port);
                saddr.sin_addr.s_addr = 0;    
                if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
                {
                    listen(s, 10);
                    int tmp = accept(s, (SOCKADDR*)&caddr, &clen);
                    close(s);
                    if (tmp > 0)
                    {
                        FILE* f = fopen(filename, "rb");
                        if (f != NULL)
                        {
                            while (!feof(f))
                            {
                                memset(buffer, 0, sizeof(buffer));
                                int r = fread(buffer, 1, sizeof(buffer), f);
                                if (r > 0)
                                {
                                    send(tmp, buffer, r, 0);
                                }
                            }
                            fclose(f);
                            close(tmp);
                            char* response = "DONE\n";
                            send(c, response, strlen(response), 0);        
                        }else
                        {
                            char* response = "Failed to open file\n";
                            send(c, response, strlen(response), 0);        
                        }
                    }else
                    {
                        char* response = "Failed to create data connection\n";
                        send(c, response, strlen(response), 0);    
                    }           
                }else
                {
                    char* response = "Failed to open port\n";
                    send(c, response, strlen(response), 0);
                }
            }
        }else
            break;
    }
    close(c);
}

int main()
{
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = 0;
    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(s, 10);
        while (0 == 0)
        {
            int c = accept(s, (SOCKADDR*)&caddr, &clen);
            int* arg = (int*)calloc(1,sizeof(int));
            *arg = c;
            pthread_t tid;
            pthread_create(&tid, NULL, ClientThread, (void*)arg);
        }
    }else
    {
        printf("Failed to bind!\n");
    }
    close(s);
}