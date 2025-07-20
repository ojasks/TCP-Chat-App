#include "socketutil.h"
#include <unistd.h>  // for close()


struct sockaddr_in* createIPv4Address(char *ip, int port) {
    struct sockaddr_in* address = malloc(sizeof(struct sockaddr_in));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(address->sin_addr));
    return address;
}

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createClientSocket(const char* ip, int port) {
    int sock = createTCPIpv4Socket();
    if (sock < 0) return -1;

    struct sockaddr_in* addr = createIPv4Address((char*)ip, port);
    if (connect(sock, (struct sockaddr*)addr, sizeof(*addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    return sock;
}
