#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 2

int clients[MAX_CLIENTS];
pthread_t threads[MAX_CLIENTS];
int numClients = 0;
int readyToCommunicate = 0;

void *client_proc(void *);

int main() {
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    while (1) {
        if (numClients < MAX_CLIENTS) {
            printf("Waiting for new client\n");
            int client = accept(listener, NULL, NULL);
            printf("New client accepted, client = %d\n", client);
            
            clients[numClients] = client;
            pthread_create(&threads[numClients], NULL, client_proc, &clients[numClients]);
            pthread_detach(threads[numClients]);
            numClients++;
            if (numClients == MAX_CLIENTS) {
                readyToCommunicate = 1;
            }
        }
    }

    return 0;
}

void *client_proc(void *arg) {
    int client = *(int *)arg;
    char buf[256];

    while (1) {
        if (readyToCommunicate) {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0) {
                // Client disconnected
                close(client);
                for (int i = 0; i < numClients; i++) {
                    if (clients[i] == client) {
                        // Remove client from the array
                        for (int j = i; j < numClients - 1; j++) {
                            clients[j] = clients[j + 1];
                            threads[j] = threads[j + 1];
                        }
                        numClients--;
                        break;
                    }
                }
                readyToCommunicate = 0;

                // Notify the other client about disconnection
                int otherClient = (client == clients[0]) ? clients[1] : clients[0];
                send(otherClient, "Client disconnected", 19, 0);
                close(otherClient);

                break;
            }

            buf[ret] = '\0';
            printf("Received from %d: %s\n", client, buf);

            // Broadcast message to other client
            for (int i = 0; i < numClients; i++) {
                if (clients[i] != client) {
                    send(clients[i], buf, ret, 0);
                }
            }
        }
    }

    pthread_exit(NULL);
}
