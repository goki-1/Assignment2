#ifndef UDP_H
#define UDP_H

// create a socket
int createsocket();


// send mesaage
int sendMessage(char* message);


// recieve message
void* receiveMessage(void *arg);


// close socket
int closeSocket();
 
#endif