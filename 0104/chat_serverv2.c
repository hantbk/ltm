#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h> // for poll()
#include <time.h>


#define MAX_FDS 2048

char name_rq[20] = "client_id: ";
char name_fd[20][20];

int main()
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
    addr.sin_port = htons(9000);

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

    struct pollfd fds[MAX_FDS];
    int nfds = 0;

    // add socket server vao poll
    fds[0].fd = listener;
    fds[0].events = POLLIN; // mask
    nfds++;

    char buf[256];

    while (1)
    {
        int ret = poll(fds, nfds, -1);

        if (fds[0].revents & POLLIN)
        {
            int client = accept(listener, NULL, NULL);
            while (1)
            {
                int vld = 1;
                send(client, name_rq, strlen(name_rq), 0);
                int ret = recv(client, name_fd[client], sizeof(name_fd[client]), 0);
                name_fd[client][ret] = 0;
                for (int j = 0; j < strlen(name_fd[client]) - 1; j++)
                {
                    if (name_fd[client][j] == ' ')
                    {
                        vld = 0;
                        printf("%d\n", j);
                        break;
                    }
                }
                if (vld == 1)
                    break;
            }
            printf("CLIENT %d CONNECTED, name client %d: %s", client, client, name_fd[client]);
            // printf("Name client %d: %s", client, name_fd[client]);
            fds[nfds].fd = client;
            fds[nfds].events = POLLIN;
            nfds++;
        }

        for (int i = 1; i < nfds; i++)
            if (fds[i].revents & (POLLIN | POLLERR))
            {
                ret = recv(fds[i].fd, buf, sizeof(buf), 0);
                
                if (ret <= 0)
                {
                    printf("%s CLOSED\n", name_fd[fds[i].fd]);
                    close(fds[i].fd);
                    if (i < nfds - 1)
                        fds[i].fd = fds[nfds - 1].fd;
                    nfds--;
                    i--;

                    continue;
                }
                buf[ret] = 0;
                // printf("%s: %s", name_fd[fds[i].fd], buf);

                // setup massage
                // clock
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                // Set to Vietnam (UTC + 7)
                timeinfo->tm_hour += 7;
                if (timeinfo->tm_hour >= 24)
                {
                    timeinfo->tm_hour -= 24;
                    timeinfo->tm_mday += 1;
                }

                printf("%02d-%02d-%04d %02d-%02d-%02d %s: %s",
                       timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                       timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name_fd[fds[i].fd], buf);

                char rcv[2048];
                sprintf(rcv, "%02d-%02d-%04d %02d-%02d-%02d %s: %s",
                        timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
                        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, name_fd[fds[i].fd], buf);

                for (int j = 1; j < nfds; j++)
                {
                    if (j != i)
                        send(fds[j].fd, rcv, strlen(rcv), 0);
                }
                bzero(rcv, 256);
            }
    }
    return 0;
}