#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int count(char *str){
    int length = strlen(str);
    int count = 0;

    for (int i = 0; i < length; i++)
    {
        if (str[i] == '0')
        {
            
        }
        
    }
    
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

    // Chap nhan ket noi
    int client = accept(listener, NULL, NULL);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }

    // Nhan thong tin file tu client
    while (1)
    {
        char buf[64];
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            break;
        }

        printf("%s", buf);
    }

    // Dong ket noi
    close(client);
    close(listener);

    return 0;
}
