#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void signalHandler(int signo)
{
    int pid = wait(NULL);
    printf("Child process done, pid = %d\n", pid);
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

    signal(SIGCHLD, signalHandler);

    while (1)
    {
        printf("Waiting for incoming connection...\n");
        int client = accept(listener, NULL, NULL);
        printf("New client connected: %d\n", client);

        if (fork() == 0)
        {
            // Tien trinh con, xu ly yeu cau tu client
            // Dong socket listener
            close(listener);

            int isLogin = 0;

            char buf[256];
            while (1)
            {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    break;

                buf[ret] = 0;
                printf("Received: %s\n", buf);

                if(!isLogin){
                    // Chua dang nhap
                    char user[32], pass[32], tmp[65], line[65];
                    int n = sscanf(buf, "%s %s %s", user, pass, tmp);
                    if(n != 2){
                        char *msg = "Sai cu phap. Hay nhap lai.\n";
                        send(client, msg, strlen(msg), 0);
                    } else
                    {
                        // Kiem tra thong tin dang nhap
                        sprintf(line, "%s %s\n", user, pass);
                        FILE *f = fopen("account.txt", "r");
                        int isFound = 0;
                        while (fgets(line, sizeof(line), f) != NULL)
                        {
                            if(strcmp(line, buf) == 0){
                                isFound = 1;
                                break;
                            }
                        }
                        if(isFound){
                            char *msg = "Dang nhap thanh cong.\n";
                            send(client, msg, strlen(msg), 0);

                            // Luu trang thai dang nhap
                            isLogin = 1;
                        } else
                        {
                            char *msg = "Dang nhap that bai.\n";
                            send(client, msg, strlen(msg), 0);
                        }
                        fclose(f);
                    } 
                } else
                {
                    // Da dang nhap
                    char cmd[300];

                    // Xoa ky tu xuong dong o cuoi buf
                    if (buf[ret - 1] == '\n')
                        buf[ret - 1] = 0;
                    
                    // Thuc hien lenh
                    sprintf(cmd, "%s > out.txt", buf);
                    system(cmd);

                    // Tra lai ket qua cho client
                    FILE *f = fopen("out.txt", "rb");
                    while(1){
                        int n = fread(buf, 1, sizeof(buf), f);
                        if(n <= 0) break;
                        send(client, buf, n, 0);
                    }
                    fclose(f);
                }
            }

            // Ket thuc tien trinh con
            exit(0);
        }
        // Dong socket client o tien trinh cha
        close(client);
    }

    getchar();
    killpg(0, SIGKILL);

    return 0;
}