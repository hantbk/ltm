#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

struct diemthi {
    int diem_qt;
    int diem_ck;
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

    int n;
    struct diemthi dt;
    dt.diem_ck = 9;
    dt.diem_qt = 8;
    
    while (1)
    {
        // printf("Nhap so: ");
        // scanf("%d", &n);

        // send(client, &n, sizeof(n), 0);
        // if (n == 0)
        // {
        //     break;
        // }

        printf("Nhap diem qua trinh: ");
        scanf("%d", &dt.diem_qt);
        printf("Nhap diem cuoi ky: ");
        scanf("%d", &dt.diem_ck);

        send(client, &dt, sizeof(dt), 0);
        if (dt.diem_ck == 0)
        {
            break;
        }
    }

    close(client);

    return 0;
}