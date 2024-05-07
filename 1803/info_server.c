#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

struct file_info
{
    char name[256];
    long size;
};

int main(){
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

    // Chap nhan ket noi
    int client = accept(listener, NULL, NULL);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }

    // Nhan thong tin file tu client
    struct file_info file[100];
    recv(client, file, sizeof(file), 0);

    // In thong tin file
    for (int i = 0; i < 100; i++)
    {
        if (file[i].size == 0)
            break;
        printf("%s - %ld bytes\n", file[i].name, file[i].size);
    }

    // Dong ket noi
    close(client);
    close(listener);

    return 0;
}
