#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef char* STRING;
short data_port = 20000;

void Append(STRING* v, const STRING x)
{
    STRING tmp = *v;
    int oldLen = tmp == NULL ? 0 : strlen(tmp);
    int newLen = oldLen + strlen(x) + 1;
    tmp = (STRING)realloc(tmp, newLen);
    memset(tmp + oldLen, 0, newLen - oldLen);
    sprintf(tmp + strlen(tmp), "%s", x);
    *v = tmp;
}

int compare(const struct dirent** item1, const struct dirent** item2)
{
    if ((*item1)->d_type == (*item2)->d_type)
    {
        return strcmp((*item1)->d_name, (*item2)->d_name);
    }else
    {
        if ((*item1)->d_type == DT_DIR)
            return -1;
        else
            return 1;
    }
}

long long getFileSize(char* folder,  char* filename)
{
    char filepath[4096] = { 0 };
    sprintf(filepath,"%s%s", folder, filename);
    FILE* f = fopen(filepath, "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        long long fs = ftell(f);
        fclose(f);
        return fs;
    }else 
        return 0;
}

void SEND(int c, char* data, int length)
{
    int sent = 0;
    while (sent < length)
    {
        int tmp = send(c, data + sent, length - sent, 0);
        if (tmp >= 0)
            sent += tmp;
        else
            break;
    }
}

void* ClientThread(void* arg)
{
    int c = *((int*)arg);
    free(arg);
    arg = NULL;
    char ownerFolder[1024] = { 0 };
    char rootPath[1024] = { 0 };
    char username[1024] = { 0 };
    char password[1024] = { 0 };
    char response[2048] = { 0 };
    int data_socket = 0;

    strcpy(response, "220-LATHEVINH Server 0.0.1\r\n");
    SEND(c, response, strlen(response));
    strcpy(response,"220 Im ready\r\n");
    SEND(c, response, strlen(response));
    while (0 == 0)
    {
        char buffer[1024] = { 0 };
        int r = recv(c, buffer, sizeof(buffer) - 1, 0);
        if (r <= 0)
        {   
            break;
        }else
        {
            if (strncmp(buffer, "USER ", 5) == 0)
            {
                int found = 0;
                char tmp[1024] = { 0 };
                sscanf(buffer + 5, "%s", tmp);
                FILE* f = fopen("ftpusers.txt","rt");
                if (f != NULL)
                {
                    while (!feof(f))
                    {
                        memset(buffer, 0, sizeof(buffer));
                        fgets(buffer, sizeof(buffer) - 1, f);
                        sscanf(buffer,"%s%s%s",username, password, rootPath);
                        if (strcmp(username, tmp) == 0)
                        {
                            memset(ownerFolder, 0, sizeof(ownerFolder));
                            strcpy(ownerFolder, rootPath);
                            found = 1;
                            break;
                        }
                    }
                    fclose(f);
                }
                if (found == 1)
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"331 Please give password\r\n");
                    SEND(c, response, strlen(response));
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"430 Invalid user or password\r\n");
                    SEND(c, response, strlen(response));
                    break;
                }
            }else if (strncmp(buffer, "PASS ", 5) == 0)
            {
                char tmp[1024] = { 0 };
                sscanf(buffer + 5, "%s", tmp);
                if (strcmp(tmp, password) == 0)
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"230 Login OKAY\r\n");
                    SEND(c, response, strlen(response));
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"430 Invalid user or password\r\n");
                    SEND(c, response, strlen(response));
                    break;
                }
            }else if (strncmp(buffer, "SYST", 4) == 0)
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"215 UNIX emulated by FileZilla.\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "FEAT", 4) == 0)
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"211-Features:\r\n MDTM\r\n REST STREAM\r\n SIZE\r\n MLST type*;size*;modify*;perm*;\r\n MLSD\r\n AUTH SSL\r\n AUTH TLS\r\n PROT\r\n PBSZ\r\n UTF8\r\n TVFS\r\n EPSV\r\n EPRT\r\n MFMT\r\n211 End\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "OPTS ", 5) == 0)
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"202 UTF8 mode is always enabled. No need to send this command\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "TYPE ", 5) == 0)
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"200 OKAY\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "PWD", 3) == 0)
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"257 \"%s\"\r\n", rootPath);
                SEND(c, response, strlen(response));
            }/*else if (strncmp(buffer, "PASV", 4) == 0)
            {
                int tmp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                SOCKADDR_IN saddr, caddr;
                int clen = sizeof(caddr);
                saddr.sin_family = AF_INET;
                saddr.sin_port = htons(data_port);
                saddr.sin_addr.s_addr = 0;
                if (bind(tmp, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
                {
                    listen(tmp, 10);
                    memset(response, 0, sizeof(response));
                    sprintf(response,"227 Entering passive mode (172,20,38,136,%d,%d)\r\n",data_port / 256, data_port % 256);
                    SEND(c, response, strlen(response));   
                    data_socket = accept(tmp, (SOCKADDR*)&caddr, &clen); 
                    close(tmp);
                    data_port += 1;
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"502 Command not supported\r\n");
                    SEND(c, response, strlen(response));        
                }
            }*/else if (strncmp(buffer, "MLSD", 4) == 0)
            {
                STRING ftpdata = NULL;
                struct dirent** output = NULL;
                int n = scandir(rootPath, &output, NULL, compare);
                if (n > 0)
                {
                    for (int i = 0;i < n;i++)
                    {
                        if (output[i]->d_type == DT_REG)
                        {
                            memset(buffer, 0, sizeof(buffer));
                            sprintf(buffer,"type=file;size=%lld;modify=20240221183348.000;perms=awr; %s\r\n",getFileSize(rootPath, output[i]->d_name), output[i]->d_name);
                            Append(&ftpdata,buffer);
                        }
                        if (output[i]->d_type == DT_DIR)
                        {
                            memset(buffer, 0, sizeof(buffer));
                            sprintf(buffer,"type=dir;modify=20230309031047.945;perms=cplem; %s\r\n", output[i]->d_name);
                            Append(&ftpdata,buffer);
                        }
                    }
                }

                if (ftpdata != NULL)
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"150 Start transferring\r\n");
                    SEND(c, response, strlen(response));    

                    SEND(data_socket, ftpdata, strlen(ftpdata));

                    memset(response, 0, sizeof(response));
                    sprintf(response,"226 Transferring completed\r\n");
                    SEND(c, response, strlen(response));    

                    close(data_socket);    
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"502 Command not supported\r\n");
                    SEND(c, response, strlen(response));        
                }
            }else if (strncmp(buffer, "CWD ", 4) == 0)
            {
                char tmp[1024] = { 0 };
                strcpy(tmp, buffer + 4);
                tmp[strlen(tmp) - 1] = 0;
                tmp[strlen(tmp) - 1] = 0;
                sprintf(rootPath, "%s%s/", rootPath, tmp);
                memset(response, 0, sizeof(response));
                sprintf(response,"250 OKAY\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "CDUP", 4) == 0)
            {
                if (strcmp(rootPath, ownerFolder) != 0)
                {
                    rootPath[strlen(rootPath) - 1] = 0;
                    while (rootPath[strlen(rootPath) - 1] != '/')
                    {
                        rootPath[strlen(rootPath) - 1] = 0;
                    }
                }
                
                memset(response, 0, sizeof(response));
                sprintf(response,"250 OKAY\r\n");
                SEND(c, response, strlen(response));
            }else if (strncmp(buffer, "RETR ", 5) == 0)
            {
                char filename[1024] = { 0 };
                strcpy(filename, buffer + 5);
                filename[strlen(filename) - 1] = 0;
                filename[strlen(filename) - 1] = 0;
                char filepath[4096] = { 0 };
                sprintf(filepath,"%s%s", rootPath, filename);
                FILE* f = fopen(filepath, "rb");
                if (f != NULL)
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"150 Start file transferring\r\n");
                    SEND(c, response, strlen(response));
                    while (!feof(f))
                    {
                        int r = fread(buffer, 1, sizeof(buffer), f);
                        if (r > 0)
                        {
                            SEND(data_socket, buffer, r);           
                        }else
                            break;
                    }
                    fclose(f);
                    memset(response, 0, sizeof(response));
                    sprintf(response,"226 Transferring completed\r\n");
                    SEND(c, response, strlen(response));
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"550 File not found or not accessible\r\n");
                    SEND(c, response, strlen(response));
                }

                close(data_socket);
            }else if (strncmp(buffer, "STOR ", 5) == 0)
            {
                char filename[1024] = { 0 };
                strcpy(filename, buffer + 5);
                filename[strlen(filename) - 1] = 0;
                filename[strlen(filename) - 1] = 0;
                char filepath[4096] = { 0 };
                sprintf(filepath,"%s%s", rootPath, filename);
                FILE* f = fopen(filepath, "wb");
                if (f != NULL)
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"150 Start file transferring\r\n");
                    SEND(c, response, strlen(response));
                    while (0 == 0)
                    {
                        int r = recv(data_socket, buffer, sizeof(buffer), 0);
                        if (r > 0)
                        {
                            fwrite(buffer, 1, r, f);
                        }else
                            break;
                    }
                    fclose(f);
                    memset(response, 0, sizeof(response));
                    sprintf(response,"226 Transferring completed\r\n");
                    SEND(c, response, strlen(response));
                }else
                {
                    memset(response, 0, sizeof(response));
                    sprintf(response,"550 File not found or not accessible\r\n");
                    SEND(c, response, strlen(response));
                }

                close(data_socket);
            }else if (strncmp(buffer, "PORT ", 5) == 0)
            {
                //TAO data_socket
            }
            else
            {
                memset(response, 0, sizeof(response));
                sprintf(response,"502 Command not supported\r\n");
                SEND(c, response, strlen(response));
            }
        }
    }
    close(c);
}

int main()
{
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (bind(s, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(s, 10);
        while (0 == 0)
        {
            int c = accept(s, (SOCKADDR*)&caddr, &clen);
            if (c > 0)
            {
                int* arg = (int*)calloc(1, sizeof(int));
                *arg = c;
                pthread_t tid;
                pthread_create(&tid, NULL, ClientThread, (void*)arg);
            }
        }
    }else
    {
        close(s);
        printf("Failed to bind\n");
    }
}