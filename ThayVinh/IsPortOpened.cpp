int IsPortOpened(char* IP, short port, int timeout)
{
    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, IP, &(saddr.sin_addr));

    unsigned long argp = 1;
    int result = ioctlsocket(client, FIONBIO, &argp);
    if (result != NO_ERROR)
    {
        return 0;
    }
    if (!connect(client, (struct sockaddr*)&saddr, sizeof(saddr)))
    {
        return 0;
    }
    argp = 0;
    result = ioctlsocket(client, FIONBIO, &argp);
    if (result != NO_ERROR)
    {
        return 0;
    }

    fd_set fdwrite, fderr;
    FD_ZERO(&fdwrite);
    FD_ZERO(&fderr);
    FD_SET(client, &fdwrite);
    FD_SET(client, &fderr);
    TIMEVAL timeout;
    timeout.tv_sec = timeout;
    timeout.tv_usec = 0;
    select(0, NULL, &fdwrite, &fderr, &timeout);

    if (FD_ISSET(client, &fdwrite))
    {
        closesocket(client);
        return 1;
    }

    return 0;
}
