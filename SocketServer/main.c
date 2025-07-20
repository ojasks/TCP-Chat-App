#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "socketutil.h"

struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void acceptNewConnectionAndReceiveAndPrintItsData(int serverSocketFD);
void receiveAndPrintIncomingData(int socketFD);

void startAcceptingIncomingConnections(int serverSocketFD);

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);

void sendReceivedMessageToTheOtherClients(char *buffer,int socketFD);

struct AcceptedSocket acceptedSockets[10] ;
int acceptedSocketsCount = 0;


void startAcceptingIncomingConnections(int serverSocketFD) {

    while(true)
    {
        struct AcceptedSocket* clientSocket  = acceptIncomingConnection(serverSocketFD);
        acceptedSockets[acceptedSocketsCount++] = *clientSocket;

        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}



void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket) {

    pthread_t id;
    pthread_create(&id,NULL,receiveAndPrintIncomingData,pSocket->acceptedSocketFD);
}

void receiveAndPrintIncomingData(int socketFD) {
    char buffer[1024];

    while (true)
    {
        ssize_t  amountReceived = recv(socketFD,buffer,1024,0);

        if(amountReceived>0)
        {
            buffer[amountReceived] = 0;
            printf("%s\n",buffer);

            sendReceivedMessageToTheOtherClients(buffer,socketFD);
        }

        if(amountReceived==0)
            break;
    }

    close(socketFD);
}

void sendReceivedMessageToTheOtherClients(char *buffer,int socketFD) {

    for(int i = 0 ; i<acceptedSocketsCount ; i++)
        if(acceptedSockets[i].acceptedSocketFD !=socketFD)
        {
            send(acceptedSockets[i].acceptedSocketFD,buffer, strlen(buffer),0);
        }

}

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in  clientAddress ;
    int clientAddressSize = sizeof (struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD,&clientAddress,&clientAddressSize);

    struct AcceptedSocket* acceptedSocket = malloc(sizeof (struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD>0;

    if(!acceptedSocket->acceptedSuccessfully)
        acceptedSocket->error = clientSocketFD;



    return acceptedSocket;
}


int main() {

    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("",2000);

    int result = bind(serverSocketFD,serverAddress, sizeof(*serverAddress));
    if(result == 0)
        printf("socket was bound successfully\n");

    int listenResult = listen(serverSocketFD,10);

    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD,SHUT_RDWR);

    return 0;
}





// // tcp_server.c

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <signal.h>
// #include <stdbool.h>
// #include <errno.h>
// #include <netinet/in.h>

// #define MAX_CLIENTS 10
// #define BUFFER_SIZE 1024
// #define PORT 2000

// volatile bool isRunning = true;
// pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// struct Client {
//     int socketFD;
//     struct sockaddr_in address;
// };

// struct Client clients[MAX_CLIENTS];
// int clientCount = 0;

// void shutdownServer(int signo) {
//     isRunning = false;
//     printf("\nShutting down server...\n");
//     for (int i = 0; i < clientCount; i++) {
//         close(clients[i].socketFD);
//     }
//     exit(0);
// }

// void broadcastMessage(const char *message, int senderFD) {
//     pthread_mutex_lock(&clients_mutex);
//     for (int i = 0; i < clientCount; i++) {
//         if (clients[i].socketFD != senderFD) {
//             if (send(clients[i].socketFD, message, strlen(message), 0) == -1) {
//                 perror("send failed");
//             }
//         }
//     }
//     pthread_mutex_unlock(&clients_mutex);
// }

// void *handleClient(void *arg) {
//     int socketFD = *(int *)arg;
//     free(arg);

//     char buffer[BUFFER_SIZE];
//     ssize_t bytesRead;

//     while (isRunning && (bytesRead = recv(socketFD, buffer, BUFFER_SIZE - 1, 0)) > 0) {
//         buffer[bytesRead] = '\0';
//         printf("Client %d: %s", socketFD, buffer);
//         broadcastMessage(buffer, socketFD);
//     }

//     if (bytesRead == -1) {
//         perror("recv failed");
//     } else {
//         printf("Client %d disconnected.\n", socketFD);
//     }

//     pthread_mutex_lock(&clients_mutex);
//     for (int i = 0; i < clientCount; i++) {
//         if (clients[i].socketFD == socketFD) {
//             for (int j = i; j < clientCount - 1; j++) {
//                 clients[j] = clients[j + 1];
//             }
//             clientCount--;
//             break;
//         }
//     }
//     pthread_mutex_unlock(&clients_mutex);

//     close(socketFD);
//     return NULL;
// }

// int main() {
//     signal(SIGINT, shutdownServer);

//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         perror("socket failed");
//         return 1;
//     }

//     struct sockaddr_in serverAddr = {
//         .sin_family = AF_INET,
//         .sin_addr.s_addr = INADDR_ANY,
//         .sin_port = htons(PORT)
//     };

//     if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//         perror("bind failed");
//         close(serverSocket);
//         return 1;
//     }

//     if (listen(serverSocket, MAX_CLIENTS) < 0) {
//         perror("listen failed");
//         close(serverSocket);
//         return 1;
//     }

//     printf("Server is listening on port %d...\n", PORT);

//     while (isRunning) {
//         struct sockaddr_in clientAddr;
//         socklen_t addrLen = sizeof(clientAddr);
//         int *clientSocket = malloc(sizeof(int));
//         *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);

//         if (*clientSocket < 0) {
//             perror("accept failed");
//             free(clientSocket);
//             continue;
//         }

//         pthread_mutex_lock(&clients_mutex);
//         if (clientCount >= MAX_CLIENTS) {
//             printf("Too many connections. Rejecting client.\n");
//             close(*clientSocket);
//             free(clientSocket);
//             pthread_mutex_unlock(&clients_mutex);
//             continue;
//         }
//         clients[clientCount].socketFD = *clientSocket;
//         clients[clientCount].address = clientAddr;
//         clientCount++;
//         pthread_mutex_unlock(&clients_mutex);

//         pthread_t tid;
//         if (pthread_create(&tid, NULL, handleClient, clientSocket) != 0) {
//             perror("pthread_create failed");
//             close(*clientSocket);
//             free(clientSocket);
//         } else {
//             pthread_detach(tid); // no need to join manually
//         }
//     }

//     close(serverSocket);
//     return 0;
// }
