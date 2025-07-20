#include "socketutil.h"
#include <stdlib.h> // for malloc, free

int createTCPIpv4Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* createIPv4Address(char *ip, int port) {
    struct sockaddr_in  *address = malloc(sizeof(struct sockaddr_in));
    if (!address) {
        perror("malloc failed");
        return NULL;
    }

    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if (strlen(ip) == 0) {
        address->sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, ip, &address->sin_addr.s_addr) <= 0) {
            perror("inet_pton failed");
            free(address);
            return NULL;
        }
    }

    return address;
}
