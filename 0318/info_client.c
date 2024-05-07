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

int main() {

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

    char path[256];
    // printf("Enter path: ");
    getcwd(path, sizeof(path));
    // fgets(path, sizeof(path), stdin);
    // path[strlen(path) - 1] = '\0';
    puts(path);
    DIR *d = opendir(path);
    struct dirent *dir;
    struct stat st;

    struct file_info file[100];
    int i = 0;

    if (d != NULL)
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_REG)
            {
                stat(dir->d_name, &st);
                printf("%s - %ld bytes\n", dir->d_name, st.st_size);

                strcpy(file[i].name, dir->d_name);
                file[i].size = st.st_size;
                i++;
            }
        }

    send(client, file, sizeof(struct file_info) * i, 0);

    close(client);

    return 0;
}