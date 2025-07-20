#include <stdbool.h>
#include <pthread.h>
#include "socketutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_CLIENTS 10

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD);
void* receiveAndPrintIncomingData(void* arg);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket* pSocket);
void sendReceivedMessageToOtherClients(const char* buffer, int senderSocketFD);
void startAcceptingIncomingConnections(int serverSocketFD);

struct AcceptedSocket acceptedSockets[MAX_CLIENTS];
int acceptedSocketsCount = 0;
pthread_mutex_t socketListMutex = PTHREAD_MUTEX_INITIALIZER;

void startAcceptingIncomingConnections(int serverSocketFD) {
    while (true) {
        struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);

        if (!clientSocket->acceptedSuccessfully) {
            fprintf(stderr, "Failed to accept client: %d\n", clientSocket->error);
            free(clientSocket);
            continue;
        }

        pthread_mutex_lock(&socketListMutex);
        if (acceptedSocketsCount < MAX_CLIENTS) {
            acceptedSockets[acceptedSocketsCount++] = *clientSocket;
            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
        } else {
            fprintf(stderr, "Max client limit reached. Connection refused.\n");
            close(clientSocket->acceptedSocketFD);
            free(clientSocket);
        }
        pthread_mutex_unlock(&socketListMutex);
    }
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket* pSocket) {
    pthread_t thread;
    int* pSocketFD = malloc(sizeof(int));
    *pSocketFD = pSocket->acceptedSocketFD;

    if (pthread_create(&thread, NULL, receiveAndPrintIncomingData, pSocketFD) != 0) {
        perror("Failed to create thread");
        free(pSocketFD);
    }
    pthread_detach(thread);
}

void* receiveAndPrintIncomingData(void* arg) {
    int socketFD = *((int*)arg);
    free(arg);
    char buffer[1024];

    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
        if (amountReceived <= 0) break;

        buffer[amountReceived] = '\0';
        printf("Client %d: %s\n", socketFD, buffer);

        sendReceivedMessageToOtherClients(buffer, socketFD);
    }

    close(socketFD);

    pthread_mutex_lock(&socketListMutex);
    // Remove from acceptedSockets
    for (int i = 0; i < acceptedSocketsCount; ++i) {
        if (acceptedSockets[i].acceptedSocketFD == socketFD) {
            for (int j = i; j < acceptedSocketsCount - 1; ++j)
                acceptedSockets[j] = acceptedSockets[j + 1];
            acceptedSocketsCount--;
            break;
        }
    }
    pthread_mutex_unlock(&socketListMutex);

    return NULL;
}

void sendReceivedMessageToOtherClients(const char* buffer, int senderSocketFD) {
    pthread_mutex_lock(&socketListMutex);
    for (int i = 0; i < acceptedSocketsCount; ++i) {
        int targetSocket = acceptedSockets[i].acceptedSocketFD;
        if (targetSocket != senderSocketFD) {
            send(targetSocket, buffer, strlen(buffer), 0);
        }
    }
    pthread_mutex_unlock(&socketListMutex);
}

struct AcceptedSocket* acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, &clientAddressSize);

    struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = (clientSocketFD >= 0);
    acceptedSocket->error = clientSocketFD;

    return acceptedSocket;
}

int main() {
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in* serverAddress = createIPv4Address("0.0.0.0", 2000);  // binds to all interfaces


    int bindResult = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
    if (bindResult == 0)
        printf("Socket bound to port 2000\n");
    else {
        perror("Bind failed");
        exit(1);
    }

    if (listen(serverSocketFD, MAX_CLIENTS) != 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server listening on port 2000\n");
    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    close(serverSocketFD);
    return 0;
}
