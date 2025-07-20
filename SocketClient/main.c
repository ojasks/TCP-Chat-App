#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../SocketUtil/socketutil.h"

// Correct declaration matching definition
void* listenAndPrint(void* arg);

// Thread entry point: listens for messages and prints them
void* listenAndPrint(void* arg) {
    int socketFD = *((int*)arg);
    char buffer[1024];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(socketFD, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            printf("Connection closed or error occurred.\n");
            break;
        }
        printf("Received: %s\n", buffer);
    }

    return NULL;
}

// Starts a thread that runs listenAndPrint
void startListeningAndPrintMessagesOnNewThread(int socketFD) {
    pthread_t id;
    int* arg = malloc(sizeof(int));  // Allocate memory to pass socketFD
    *arg = socketFD;

    if (pthread_create(&id, NULL, listenAndPrint, arg) != 0) {
        perror("Failed to create thread");
        free(arg);
        return;
    }

    pthread_detach(id);  // No need to join later
}

int main() {
    const char* serverIP = "127.0.0.1";
    int port = 2000;

    int socketFD = createClientSocket(serverIP, port);
    if (socketFD < 0) {
        fprintf(stderr, "Failed to connect to server.\n");
        return 1;
    }

    printf("Connected to server at %s:%d\n", serverIP, port);

    startListeningAndPrintMessagesOnNewThread(socketFD);

    char buffer[1024];
    while (1) {
        printf("Enter message: ");
        fgets(buffer, sizeof(buffer), stdin);

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        write(socketFD, buffer, strlen(buffer));
    }

    close(socketFD);
    return 0;
}
