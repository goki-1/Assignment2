#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "hal/udp.h"

#define SUCCESS 0
#define FAIL 1
#define MAXBUFLEN 1024
#define PORT 12345

struct sockaddr_in sin;
memset(&sin, 0, sizeof(sin));
sin.sin_family = AF_INET;
sin.sin_addr.s_addr = htonl(INADDR_ANY);
sin.sin_port = htons(PORT);

// Function to create a UDP socket
int createsocket(char* hostname, char* remoteport, char* myport) 
{
    int socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

    // Resolve remote address
   
}

// Function to bind the socket to a local port
int bindsocket()
{
    if (sockfd == -1) {
        fprintf(stderr, "bindsocket: Socket not initialized.\n");
        return FAIL;
    }

    if (bind(sockfd, recvres->ai_addr, recvres->ai_addrlen) == -1) {
        perror("bindsocket: bind failed");
        return FAIL;
    }

    return SUCCESS;
}

// Function to close the socket and free resources
int closeSocket()
{
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
    
    if (res) {
        freeaddrinfo(res);
        res = NULL;
    }

    if (recvres) {
        freeaddrinfo(recvres);
        recvres = NULL;
    }

    return SUCCESS;
}
