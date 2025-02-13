#ifndef UDP_H
#define UDP_H

#define RECVBUFLEN 64     //we will only take smaller messages
#define SENDBUFLEN 1024   //we need to send alot of data so we need a bigger buffer

//structs to pass data to thread functions i.e recv and send
typedef struct{
    char* buff;
    int buff_len;
}thread_data;

// create a socket
//return 1 on success
//0 on failure
int init_socket();

// get socket descriptor
//returns the socket descriptor just a helper function
//returns -1 on failure
int get_socket_descriptor();

//recieves the message from the socket
void *recv_message(thread_data *recv_data);    

// send message to the socket
void *send_message(thread_data *send_data);

// close socket
int close_socket();
 
#endif