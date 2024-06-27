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
    char rootPath[1024] = { 0 };
    STRING header = NULL;

    while (0 == 0)
    {
        while (header == NULL || strstr(header, "\r\n\r\n") == NULL)
        {
            char tmp[2] = { 0 };
            int r = recv(c, tmp, 1, 0);
            if (r <= 0)
            {
                close(c);
                return NULL;
            }else
            {
                Append(&header, tmp);
            }
        }

        char command[16] = { 0 };
        char uri[1024] = { 0 };
        char version[16] = { 0 };
        char tmp[1024] = { 0 };

        sscanf(header, "%s%s%s", command, uri, version);

        while (strstr(uri,"%20") != NULL)
        {
            char* found = strstr(uri, "%20");
            
            strncpy(tmp, uri, found - uri);
            tmp[strlen(tmp)] = ' ';
            strcpy(tmp + strlen(tmp), found + 3);
            strcpy(uri, tmp);
            memset(tmp, 0, sizeof(tmp));
        }

        if (strcmp(command,"GET") == 0)
        {
            if (strcmp(uri,"/") == 0)
            {
                STRING html = NULL;
                Append(&html, "<html>");
                struct dirent** output = NULL;
                int n = scandir("/", &output, NULL, compare);
                if (n > 0)
                {
                    for (int i = 0;i < n;i++)
                    {
                        if (output[i]->d_type == DT_REG)
                        {
                            Append(&html,"<a href=\"");
                            Append(&html, output[i]->d_name);
                            Append(&html, "*");
                            Append(&html, "\">");
                            Append(&html, "<i>");
                            Append(&html, output[i]->d_name);
                            Append(&html, "</i></a><br>");
                        }
                        if (output[i]->d_type == DT_DIR)
                        {
                            Append(&html,"<a href=\"");
                            Append(&html, output[i]->d_name);
                            Append(&html, "\">");
                            Append(&html, "<b>");
                            Append(&html, output[i]->d_name);
                            Append(&html, "</b></a><br>");
                        }
                    }
                }else
                {
                    Append(&html,"<b><H>No item found!</H></b>");
                }
                Append(&html, "</html>");
                char Response[1024] = { 0 };
                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(html));
                SEND(c, Response, strlen(Response)); 
                SEND(c, html, strlen(html));
                free(html);
                html = NULL;
                close(c);
            }else
            {
                if (uri[strlen(uri) - 1] == '*' || strstr(uri, "favicon.ico") != NULL)
                {
                    if (uri[strlen(uri) - 1] == '*')
                    {
                        uri[strlen(uri) - 1] = 0;
                        char buffer[1024] = { 0 };
                        FILE* f = fopen(uri, "rb");
                        if (f != NULL)
                        {
                            fseek(f, 0, SEEK_END);
                            int flen = ftell(f);
                            fseek(f, 0, SEEK_SET);

                            char Response[1024] = { 0 };
                            int dot = strlen(uri) - 1;
                            while (uri[dot] != '.')
                            {
                                dot -= 1;
                            }

                            if (strcasecmp(uri + dot, ".txt") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".wav") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: audio/wav\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".png") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".jpg") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".jpeg") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".mp3") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: audio/mp3\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".mp4") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nContent-Length: %d\r\n\r\n", flen);
                            }else if (strcasecmp(uri + dot, ".pdf") == 0)
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: application/pdf\r\nContent-Length: %d\r\n\r\n", flen);
                            }else
                            {
                                sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %d\r\n\r\n", flen);
                            }

                            char* data = (char*)calloc(flen, 1);
                            fread(data, 1, flen, f);
                            fclose(f);

                            send(c, Response, strlen(Response), 0); 
                            int sent = 0;
                            while (sent < flen)
                            {
                                sent += send(c, data + sent, flen - sent, 0);
                            }
                            free(data);

                            close(c);
                        }else{
                            char Response[1024] = { 0 };
                            sprintf(Response,"HTTP/1.1 404 NOT FOUND\r\n\r\n");
                            SEND(c, Response, strlen(Response)); 
                            close(c);
                        }
                    }else
                    {
                        char buffer[1024] = { 0 };
                        FILE* f = fopen("/mnt/c/temp/mysite.ico","rb");
                        fseek(f, 0, SEEK_END);
                        int flen = ftell(f);
                        fseek(f, 0, SEEK_SET);
                        char Response[1024] = { 0 };
                        sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\nContent-Length: %d\r\n\r\n", flen);
                        SEND(c, Response, strlen(Response)); 
                        while (!feof(f))
                        {
                            int r = fread(buffer, 1, 1024, f);
                            if (r > 0)
                            {
                                SEND(c, buffer, r);
                            }else
                                break;
                        }
                        fclose(f);
                        close(c);
                    }
                }else
                {
                    STRING html = NULL;
                    Append(&html, "<html>");
                    struct dirent** output = NULL;
                    int n = scandir(uri, &output, NULL, compare);
                    if (n > 0)
                    {
                        for (int i = 0;i < n;i++)
                        {
                            if (output[i]->d_type == DT_REG)
                            {
                                Append(&html,"<a href=\"");
                                Append(&html, uri);
                                if (uri[strlen(uri) - 1] != '/')
                                {
                                    Append(&html, "/");
                                }
                                Append(&html, output[i]->d_name);
                                Append(&html, "*");
                                Append(&html, "\">");
                                Append(&html, "<i>");
                                Append(&html, output[i]->d_name);
                                Append(&html, "</i></a><br>");
                            }
                            if (output[i]->d_type == DT_DIR)
                            {
                                Append(&html,"<a href=\"");
                                Append(&html, uri);
                                if (uri[strlen(uri) - 1] != '/')
                                {
                                    Append(&html, "/");
                                }
                                Append(&html, output[i]->d_name);
                                Append(&html, "\">");
                                Append(&html, "<b>");
                                Append(&html, output[i]->d_name);
                                Append(&html, "</b></a><br>");
                            }
                        }
                    }else
                    {
                        Append(&html,"<b><H>No item found!</H></b>");
                    }
                    Append(&html, "<br><form action=\"");
                    Append(&html, uri);
                    Append(&html, "\" method=\"POST\" enctype=\"multipart/form-data\">");
                    Append(&html,"<input type=\"file\" name=\"file1\"/><br>");
                    Append(&html,"<input type=\"file\" name=\"file2\"/><br><br>");
                    Append(&html, "<input type=\"submit\" value=\"Upload File\"/>");
                    Append(&html, "</form>");

                    Append(&html, "</html>");
                    char Response[1024] = { 0 };
                    sprintf(Response,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", (int)strlen(html));
                    SEND(c, Response, strlen(Response)); 
                    SEND(c, html, strlen(html));
                    free(html);
                    html = NULL;
                    close(c);
                }
            }
        }else if (strcmp(command,"POST") == 0)
        {
            char* ctlen = strstr(header, "Content-Length: ") + strlen("Content-Length: ");
            int len = 0;
            sscanf(ctlen, "%d", &len);
            char* data = (char*)calloc(len, 1);
            int received = 0;
            while (received < len)
            {
                int r = recv(c, data + received, len - received, 0); 
                if (r <= 0)
                {
                    break;
                }else
                {
                    received += r;
                }
            }
            
            int start = 0;
            char filename[1024] = { 0 };
            while (start < len)
            {
                while ( start < len && 
                        (data[start] != '\r' || 
                        data[start + 1] != '\n' ||
                        data[start + 2] != '\r' ||
                        data[start + 3] != '\n'))
                {
                    start += 1;
                }
                if (start < len)
                {
                    memset(filename, 0, sizeof(filename));
                    data[start + 3] = 0;
                    int tmp = start;
                    while (strstr(data + tmp,"filename") == NULL)
                    {
                        tmp -= 1;
                    }
                    int tmp1 = tmp + 10;
                    while (data[tmp1] != '\"')
                    {
                        tmp1 += 1;
                    }
                    strncpy(filename, data + tmp + 10, tmp1 - tmp - 10);
                    char fpath[2048] = { 0 };
                    sprintf(fpath, "%s/%s", uri, filename);
                    start += 4;
                    int end = start;
                    while ( end < len && 
                            (data[end] != '\r' || 
                            data[end + 1] != '\n' || 
                            data[end + 2] != '-' ||
                            data[end + 3] != '-'))
                    {
                        end += 1;
                    }
                    if (end < len)
                    {
                        FILE* f = fopen(fpath,"wb");
                        fwrite(data + start, 1, end - start, f);
                        fclose(f);            
                    }
                    start = end;
                }
            }
        }

        free(header);
        header = NULL;
    }
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