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
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    int client = accept(listener, NULL, NULL);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }

    FILE *f1 = fopen(argv[2], "rb");
    char buf[256];
    while (1)
    {
        int ret = fread(buf, 1, sizeof(buf), f1);
        if (ret <= 0)
            break;
        send(client, buf, ret, 0);
    }

    FILE *f2 = fopen(argv[3], "a");
    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;

        char data[300];
        sprintf(data, "%s\n", buf);
        fwrite(data, 1, ret, f2);
    }

    fclose(f2);
    fclose(f1);
    close(client);
    close(listener);
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// int main(int argc, char *argv[])
// {
//     int port = atoi(argv[1]);
//     char *hello_file = argv[2];
//     char *log_file = argv[3];

//     // Tao socket cho ket noi
//     int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (listener == -1)
//     {
//         perror("socket() failed");
//         return 1;
//     }

//     // Khai bao dia chi server
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);
//     addr.sin_port = htons(port);

//     // Gan socket voi cau truc dia chi
//     if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
//     {
//         perror("bind() failed");
//         return 1;
//     }

//     // Chuyen socket sang trang thai cho ket noi
//     if (listen(listener, 5))
//     {
//         perror("listen() failed");
//         return 1;
//     }

    
//     FILE *f1 = fopen(hello_file, "rb");
//     char buf[256];

//     while (1)
//     {
//         int client = accept(listener, NULL, NULL);
//         if (f1 == NULL)
//         {
//             printf("Failed to open file\n");
//             continue;
//         }

//         int ret1 = fread(buf, 1, sizeof(buf), f1);
//         if (ret1 <= 0)
//         {
//             perror("Error");
//         }
//         int n = send(client, buf, ret1, 0);
//         if (n == -1)
//         {
//             perror("send() failed");
//             break;
//         }

//         FILE *f2 = fopen(argv[3], "wb");
//         while (1)
//         {
//             bzero(buf, 256);
//             int ret2 = recv(client, buf, sizeof(buf), 0);
//             if (ret2 <= 0)
//             {
//                 close(client);
//                 continue;
//             }
//             printf("client: %s\n", buf);
//             fwrite(buf, 1, ret2, f2);
//             printf("SAVED\n");
//         }
//         fclose(f2);
//         close(client);
//     }
//     fclose(f1);
//     close(listener);

//     return 0;
// }