#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h> // for poll()

#define MAX_FDS 2048

char username[20] = "username: ";
char password[20] = "password: ";
char success[35] = "Login successfully, enter command:";
char user_rcv[20] = "", pass_rcv[20] = "";

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
    FILE *f = fopen("accounts.txt", "rb");

    while (1)
    {
        int ret = poll(fds, nfds, -1);

        if (fds[0].revents & POLLIN)
        {
            int client = accept(listener, NULL, NULL);

            char line[100];
            int invld = 1;
            while (1)
            {
                // Nhan username va password tu client
                send(client, username, strlen(username), 0);
                int ret1 = recv(client, user_rcv, sizeof(user_rcv), 0);
                // user_rcv[ret1] = '\0';
                user_rcv[strlen(user_rcv) - 1] = '\0'; 
                // printf("Username: %s\n", user_rcv);

                send(client, password, strlen(password), 0);
                int ret2 = recv(client, pass_rcv, sizeof(pass_rcv), 0);
                // pass_rcv[ret2] = '\0';
                pass_rcv[strlen(pass_rcv) - 1] = '\0'; 
                // printf("Password: %s\n", pass_rcv);

                while (fgets(line, sizeof(line), f))
                {
                    char *p_user = strtok(line, " \n");
                    char *p_pass = strtok(NULL, " \n");

                    if (p_user != NULL && p_pass != NULL)
                    {
                        if (strcmp(p_user, user_rcv) == 0 && strcmp(p_pass, pass_rcv) == 0)
                        {
                            // printf("Login successfully\n");
                            invld = 0;
                            break;
                        }
                    }
                }
                if (invld == 0)
                {
                    fclose(f);
                    break;
                }
            }
            send(client, success, strlen(success), 0);
            printf("CLIENT %d CONNECTED, name: %s\n", client, user_rcv);
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
                    printf("CLIENT %d CLOSED\n", fds[i].fd);
                    close(fds[i].fd);

                    if (i < nfds - 1)
                        fds[i].fd = fds[nfds - 1].fd;
                    nfds--;
                    i--;

                    continue;
                }

                char tmp[2048];
                buf[ret] = 0;

                // Xu ly lenh
                sprintf(tmp, "%s > out.txt", buf);
                system(tmp);

                FILE *f = fopen("out.txt", "rb");
                while (!feof(f))
                {
                    int ret = fread(tmp, 1, sizeof(tmp), f);
                    if (ret <= 0)
                        break;
                    send(fds[i].fd, tmp, ret, 0);
                }
                fclose(f);
                printf("Client %d: %s", fds[i].fd, buf);
            }
    }
    return 0;
}