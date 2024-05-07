#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

int remove_client(int client, int *client_socket, char **name_clients, int *num_clients)
{
    // Remove the client from the list of connected clients
    int i = 0;
    for (; i < *num_clients; i++)
    {
        if (client_socket[i] == client)
        {
            break;
        }
    }
    if (i < *num_clients)
    {
        if (i < *num_clients - 1)
        {
            client_socket[i] = client_socket[*num_clients - 1];
            strcpy(name_clients[i], name_clients[*num_clients - 1]);
        }
        free(name_clients[*num_clients - 1]);
        (*num_clients)--;
    }
    return 1;
}

int main()
{
    // Create a socket for connection
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // Server address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // Bind socket to address
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    // Change socket to listening state
    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    // Declare fdread set to store sockets and fdtest set for event polling
    fd_set fdread, fdtest;
    FD_ZERO(&fdread);

    // Add sockets to fread set
    FD_SET(listener, &fdread);

    char buf[256];

    int client_sockets[FD_SETSIZE];
    char *name_clients[FD_SETSIZE];
    int num_clients = 0;

    while (1)
    {
        // Preserve sockets in the fdread set
        fdtest = fdread;

        // Wait for events or timeout
        int ret = select(FD_SETSIZE, &fdread, NULL, NULL, NULL);
        if (ret == -1)
        {
            printf("select() failed.\n");
            break;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            // Check for socket events
            if (FD_ISSET(i, &fdtest))
            {
                // Listener socket has a connection request
                if (i == listener)
                {
                    int client = accept(listener, NULL, NULL);
                    if (client >= FD_SETSIZE) // Exceeded maximum connections
                    {
                        close(client);
                    }
                    else // Accept connection
                    {
                        FD_SET(client, &fdread);
                        printf("New client connected: %d\n", client);

                        // Send greeting with the number of connected clients
                        char greeting[256];
                        sprintf(greeting, "Xin chao. Hien co %d clients dang ket noi.\n", num_clients);
                        send(client, greeting, strlen(greeting), 0);

                        // Add client to the list
                        client_sockets[num_clients] = client;
                        name_clients[num_clients] = NULL; // Placeholder for client name
                        num_clients++;
                    }
                }
                else // Client socket has data to receive
                {
                    int client = i;
                    int ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        close(client);
                        FD_CLR(client, &fdread);
                        printf("Client %d disconnected\n", client);

                        // Remove client from the list
                        remove_client(client, client_sockets, name_clients, &num_clients);
                        continue;
                    }

                    buf[ret] = '\0'; // Null-terminate received data

                    // Check for exit command
                    if (strcmp(buf, "exit") == 0)
                    {
                        // Send farewell message and close connection
                        char farewell[] = "Tam biet. Hen gap lai!\n";
                        send(client, farewell, strlen(farewell), 0);
                        close(client);
                        FD_CLR(client, &fdread);

                        // Remove client from the list
                        remove_client(client, client_sockets, name_clients, &num_clients);
                        // Remove the break statement to continue processing events
                    }

                    // Encrypt and send the received message back to the client
                    for (int j = 0; j < ret; j++)
                    {
                        if (buf[j] >= 'a' && buf[j] < 'z')
                            buf[j]++;
                        else if (buf[j] == 'z')
                            buf[j] = 'a';
                        else if (buf[j] >= 'A' && buf[j] < 'Z')
                            buf[j]++;
                        else if (buf[j] == 'Z')
                            buf[j] = 'A';
                        else if (buf[j] >= '0' && buf[j] <= '9')
                            buf[j] = '9' - buf[j] + '0';
                        // else leave other characters unchanged
                    }

                    send(client, buf, strlen(buf), 0);
                }
            }
        }
    }

    close(listener);

    return 1;
}
