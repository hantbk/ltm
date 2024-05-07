#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

void signalHandler(int signo)
{
    int pid = wait(NULL);
    printf("Child process done, pid = %d\n", pid);
}

int main()
{

    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    signal(SIGCHLD, signalHandler);

    while (1)
    {
        printf("Waiting for incoming connection...\n");
        int client = accept(listener, NULL, NULL);
        printf("New client connected: %d\n", client);

        if (fork() == 0)
        {
            // Tien trinh con, nhan du lieu tu client
            // Dong socket ket noi o tien trinh con
            close(listener);

            char buf[256];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                {
                    break;
                }
                buf[ret] = '\0';
                printf("Received: %s\n", buf);
            }
            // Dong socket ket noi o tien trinh con
            exit(0);
        }

        // Dong socket ket noi o tien trinh cha
        close(client);
    }
  
    return 0;
}