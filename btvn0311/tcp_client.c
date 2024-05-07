#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char *argv[])
{
    
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    char buf[256];

    // reading from server
    int n = read(client, buf, 256);
    if (n < 0)
    {
        perror("read() failed: ");
        exit(1);
    }
    printf("Server: %s\n", buf);

    while (1)
    {
        printf("Nhap du lieu: ");
        fgets(buf, 256, stdin);
        int n = write(client, buf, strlen(buf));
        if (n <= 0)
        {
            perror("write() failed");
            return 1;
        }
    }

    close(client);
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <arpa/inet.h>
// #include <unistd.h>
// #include <errno.h>

// int main(int argc, char *argv[])
// {
//     int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     char *ip = argv[1];
//     int port = atoi(argv[2]);

//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = inet_addr(ip);
//     addr.sin_port = htons(port);

//     int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
//     if (ret == -1)
//     {
//         perror("connect() failed");
//         return 1;
//     }

//     char buf[256];

//     int re = recv(client, buf, sizeof(buf), 0);
//     if (re <= 0)
//     {
//         perror("Error");
//         return 1;
//     }
//     printf("%s\n", buf);

//     while (1)
//     {
//         printf("Enter message: ");
//         fgets(buf, sizeof(buf), stdin);
//         if (strncmp(buf, "exit", 4) == 0)
//         {
//             printf("Exit");
//             break;
//         }
//         int n = write(client, buf, strlen(buf));
//         if (n == -1)
//         {
//             perror("Send() failed");
//             break;
//         }
//     }

//     close(client);

//     return 0;
// }