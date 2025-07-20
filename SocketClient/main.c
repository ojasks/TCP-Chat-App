
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "socketutil.h"

void startListeningAndPrintMessagesOnNewThread(int fd);

void listenAndPrint(int socketFD);

void readConsoleEntriesAndSendToServer(int socketFD);

int main() {

    int socketFD = createTCPIpv4Socket();
    struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);


    int result = connect(socketFD,address,sizeof (*address));

    if(result == 0)
        printf("connection was successful\n");

    startListeningAndPrintMessagesOnNewThread(socketFD);

    readConsoleEntriesAndSendToServer(socketFD);


    close(socketFD);

    return 0;
}

void readConsoleEntriesAndSendToServer(int socketFD) {
    char *name = NULL;
    size_t nameSize= 0;
    printf("please enter your name?\n");
    ssize_t  nameCount = getline(&name,&nameSize,stdin);
    name[nameCount-1]=0;


    char *line = NULL;
    size_t lineSize= 0;
    printf("type and we will send(type exit)...\n");


    char buffer[1024];

    while(true)
    {


        ssize_t  charCount = getline(&line,&lineSize,stdin);
        line[charCount-1]=0;

        sprintf(buffer,"%s:%s",name,line);

        if(charCount>0)
        {
            if(strcmp(line,"exit")==0)
                break;

            ssize_t amountWasSent =  send(socketFD,
                                          buffer,
                                          strlen(buffer), 0);
        }
    }
}

void startListeningAndPrintMessagesOnNewThread(int socketFD) {

    pthread_t id ;
    pthread_create(&id,NULL,listenAndPrint,socketFD);
}

void listenAndPrint(int socketFD) {
    char buffer[1024];

    while (true)
    {
        ssize_t  amountReceived = recv(socketFD,buffer,1024,0);

        if(amountReceived>0)
        {
            buffer[amountReceived] = 0;
            printf("Response was %s\n ",buffer);
        }

        if(amountReceived==0)
            break;
    }

    close(socketFD);
}







// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <arpa/inet.h>
// #include <signal.h>

// #define PORT 12345
// #define MAX_LEN 1024

// volatile sig_atomic_t isRunning = 1; // Shared between threads

// int socketFD;

// void handle_sigint(int sig) {
//     isRunning = 0;
//     shutdown(socketFD, SHUT_RDWR);  // Stop socket I/O
//     close(socketFD);
//     printf("\nDisconnected from server. Exiting...\n");
//     exit(0);
// }

// void* receive_messages(void* arg) {
//     char buffer[MAX_LEN];
//     while (isRunning) {
//         memset(buffer, 0, sizeof(buffer));
//         ssize_t bytes = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
//         if (bytes <= 0) {
//             if (isRunning) {
//                 perror("recv failed");
//             }
//             isRunning = 0;
//             break;
//         }

//         // Handle message delimiter (strip newline)
//         buffer[strcspn(buffer, "\n")] = 0;
//         printf("Server: %s\n", buffer);
//     }
//     return NULL;
// }

// int main() {
//     struct sockaddr_in serverAddr;
//     pthread_t recvThread;

//     // Handle Ctrl+C to shutdown gracefully
//     signal(SIGINT, handle_sigint);

//     socketFD = socket(AF_INET, SOCK_STREAM, 0);
//     if (socketFD < 0) {
//         perror("socket failed");
//         exit(EXIT_FAILURE);
//     }

//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(PORT);
//     serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

//     if (connect(socketFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
//         perror("connect failed");
//         exit(EXIT_FAILURE);
//     }

//     printf("Connected to server. Type your messages below:\n");

//     if (pthread_create(&recvThread, NULL, receive_messages, NULL) != 0) {
//         perror("pthread_create failed");
//         exit(EXIT_FAILURE);
//     }

//     char message[MAX_LEN];
//     while (isRunning) {
//         if (fgets(message, sizeof(message), stdin) == NULL) break;

//         // Append newline delimiter
//         if (message[strlen(message) - 1] != '\n')
//             strcat(message, "\n");

//         ssize_t sent = send(socketFD, message, strlen(message), 0);
//         if (sent == -1) {
//             perror("send failed");
//             isRunning = 0;
//             break;
//         }

//         if (strncmp(message, "/exit", 5) == 0) {
//             isRunning = 0;
//             break;
//         }
//     }

//     pthread_join(recvThread, NULL);
//     close(socketFD);
//     return 0;
// }
