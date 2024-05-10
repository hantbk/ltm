// #include <stdio.h>
// #include <stdlib.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <netdb.h>
// #include <string.h>

// int main()
// {

//     struct addrinfo *res, *p;
//     int ret = getaddrinfo("gmail.com", "http", NULL, &res);

//     if (ret != 0)
//     {
//         printf("Failed to get IP\n");
//         return 1;
//     }

//     p = res;
//     while (p != NULL)
//     {
//         if (p->ai_family == AF_INET)
//         {
//             printf("IPv4\n");
//             struct sockaddr_in addr;
//             memcpy(&addr, p->ai_addr, p->ai_addrlen);
//             printf("IP: %s\n", inet_ntoa(addr.sin_addr));
//         }
//         else if (p->ai_family == AF_INET6)
//         {
//             printf("IPv6\n");
//             char buf[64];
//             struct sockaddr_in6 addr6;
//             memcpy(&addr6, p->ai_addr, p->ai_addrlen);
//             printf("IP: %s\n", inet_ntop(p->ai_family, &addr6.sin6_addr, buf, sizeof(addr6)));
//         }
//         p = p->ai_next;
//     }
//     // freeaddrinfo(res);
//     return 0;
// }

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

int main()
{
    struct addrinfo *res, *p;
    int ret = getaddrinfo("nukeviet.vn", "http", NULL, &res);
    if (ret != 0)
    {
        printf("Khong phan giai duoc\n");
        return 1;
    }

    // Duyet danh sach ket qua
    p = res;
    while (p != NULL)
    {
        if (p->ai_family == AF_INET)
        {
            printf("IPv4\n");
            struct sockaddr_in addr;
            memcpy(&addr, p->ai_addr, sizeof(addr));
            printf("IP: %s\n", inet_ntoa(addr.sin_addr));
        }
        else if (p->ai_family == AF_INET6)
        {
            printf("IPv6\n");
            char buf[64];
            struct sockaddr_in6 addr6;
            memcpy(&addr6, p->ai_addr, sizeof(addr6));
            printf("IP: %s\n", inet_ntop(p->ai_family, &addr6.sin6_addr, buf, sizeof(buf)));
        }
        p = p->ai_next;
    }
    freeaddrinfo(res);
}