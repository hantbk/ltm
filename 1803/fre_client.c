#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

struct file_info
{
    char name[256];
    long size;
};

int main()
{

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    char filename[64];
    printf("Enter filename: ");
    fgets(filename, sizeof(filename), stdin);
    FILE *f = fopen(filename, "rb");

    char buf[64];
    while (1)
    {
        int ret = fread(buf, 1, sizeof(buf), f);
        if (ret <= 0)
        {
            break;
        }
        send(client, buf, ret, 0);
    }
    
    close(client);

    return 0;
}