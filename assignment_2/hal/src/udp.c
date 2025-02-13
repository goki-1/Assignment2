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
#define PORT 12345

//global variables
static struct sockaddr_in sin;
static int socketDescriptor = -1;
static struct sockaddr_in sinRemote;
static unsigned int sin_len = sizeof(sinRemote);
static char message_rc[RECVBUFLEN];
static char message_sn[SENDBUFLEN];


// Function to create and bind  UDP socket
int init_socket() 
{
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketDescriptor == -1) {
        perror("socket failure \n");
        return FAIL;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    //this is where we put the port for binding the socket
    sin.sin_port = htons(PORT);

    if (bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind failure \n");
        return FAIL;
    }
    return SUCCESS;  
}

int get_socket_descriptor()
{
    return socketDescriptor;
}

// Function to receive message from the socket
//it will not run as an infinite loop we need to call it again and again
void* recv_message(thread_data *recv_data)
{
    //copying the buffer and buffer length to local variables
    char* buffer = recv_data->buff;
    int buffer_length = recv_data->buff_len;
    if(socketDescriptor == -1)
    {
        perror("socket descriptor is not initialized \n");
        return NULL;
    }
    //we will keep on recieving the message until we get a message
    int bytes_received = recvfrom(socketDescriptor, message_rc, RECVBUFLEN - 1, 0, (struct sockaddr *)&sinRemote, &sin_len);
    
    if(bytes_received == -1){
        perror("recieve error \n");
        return NULL;
    }
    
    message_rc[bytes_received] = 0; //null terminating the buffers
    if(buffer_length < bytes_received)
    {
        perror("buffer length is smaller than the message length \n");
        return NULL;
    }
    //if no error then copy to the buffer passed by the user 
    snprintf(buffer, buffer_length, "%s", message_rc);
    return NULL;
}

// Function to send message to the socket
//buffer is the message to be sent
//buffer_length is the length of the message
void* send_message(thread_data *send_data)
{
    char* buffer = send_data->buff;
    int buffer_length = send_data->buff_len;
    if(buffer_length > SENDBUFLEN)
    {
        perror("buffer length is greater than the message length \n");
        return NULL;
    }
    //we will recieve the message from the buffer and we will send it via the socket
    int bytes_sent = sendto(socketDescriptor, buffer, buffer_length, 0, (struct sockaddr *)&sinRemote, sin_len);
    if (bytes_sent == -1) {
        perror("sending error \n");
        return NULL;
    }
    return NULL;
}

// Function to close the socket and free resources
int close_socket()
{
   if(close(socketDescriptor))
   {
        return SUCCESS;
   }
    return FAIL;
   
}
