#include "hal/connection.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
//this macro is for fragmenting the message to be sent
#define UDP_LIMIT 1500

//this is the thread that will continuously listen to the incoming messages and respond to them and the mutex to change the curr and prev command 
pthread_t communication_thread; 
pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;

//variables to store the curr and previous command
static Command_type curr_command;
static Command_type prev_command;
//boolean flag in order to check for incoming messages
static bool is_first = true;
//the struct that will hold the buffer and the length og the buffer
buffer_info recv_data;
buffer_info send_data;
//buffer to store the incoming and outgoing messages
char recv_buffer[RECVBUFLEN];
char send_buffer[SENDBUFLEN];

//these are the variables that will be updated by the main program
//check if this work with static or not
static long long total_count = 0;
static int length_ = 0, dips_ = 0, history_size_ = 0;
static double* history_ = NULL;

//converts the message to lower case so that the commands can be parsed easily
static void lower(char *message){
    if(message == NULL) return;
    for(int i=0; message[i] != '\0'; i++){
        message[i] = tolower(message[i]);
    }
}

//this function will be used in the thread to receive the message, parse it and then send the response after generating it
void* comm_func(void* data){
    //typecasting the void pointer to the buffer_info struct
    buffer_info*  cach = (buffer_info*) data;
    Command_type comm = CMD_HELP;
    while(comm != CMD_STOP){
        recv_message(data);                             //receive the message
        comm = parse_command(cach->buff);               //parse the message to get the command
        generate_response(comm);                        //generate the response
        send_message(&send_data);                       //send the response
    }

    return NULL;
}

//this function will be used to start the connection and create the thread
void start_connection(void){
    if(init_socket() == FAIL){
        perror("socket initialization failed\n");
        exit(1);
    }
    //initializing the buffer_info struct
    recv_data.buff = recv_buffer;
    recv_data.buff_len = RECVBUFLEN;
    send_data.buff = send_buffer;
    send_data.buff_len = SENDBUFLEN;

    //pthread_mutex_init(&command_mutex, NULL);
    if(pthread_create(&communication_thread, NULL, comm_func, (void*)&recv_data) != 0){
        perror("thread creation failed\n");
        exit(1);
    }
}

//this function will parse the message and return the corresponding command
Command_type parse_command(char *message){

    lower(message);
    Command_type command;

    if(*message == '\n'){
        if(is_first){
            command = CMD_HELP;
            is_first = false;
            //critical section
            pthread_mutex_lock(&command_mutex);
            //set the curr command to help command and so does the previous command
            curr_command = command;
            prev_command = curr_command;
            pthread_mutex_unlock(&command_mutex);
            return command;
        }
        else{
            pthread_mutex_lock(&command_mutex);
            //set the command to return to the previous command or curr command which would be the same in this case
            command = curr_command;
            prev_command = curr_command;
            pthread_mutex_unlock(&command_mutex);
            return command;
        }
    }
    //based on the message we will set the command
    else{
        if(strstr(message, "help") != NULL){command = CMD_HELP;}
        else if(strstr(message, "?")!=NULL){command = CMD_HELP;}
        else if(strstr(message, "count")!=NULL){command = CMD_COUNT;}
        else if(strstr(message, "length")!=NULL){command = CMD_LENGTH;}
        else if (strstr(message, "dips")!=NULL){command = CMD_DIPS;}
        else if (strstr(message, "history")!=NULL){command = CMD_HISTORY;}
        else if (strstr(message, "stop")!=NULL){command = CMD_STOP;} 
        else{command = CMD_UNKNOWN;}
    }
    //just another condition need to check if the first command is not an empty command and we need to set the is_first flag to false
    if(is_first){is_first = false;}
    //critical section
    pthread_mutex_lock(&command_mutex);
    prev_command  = curr_command;
    curr_command = command;
    pthread_mutex_unlock(&command_mutex);
    return command;
}

//this function will generate the response based on the command
void generate_response(Command_type command){
    int offset = 0, written = 0;
    switch(command){
        case CMD_COUNT:
            snprintf(send_data.buff, SENDBUFLEN, "#  samples taken total: %lld\n",total_count);
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_LENGTH:
            snprintf(send_data.buff, SENDBUFLEN, "#  samples taken last second: %d\n", length_);
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_DIPS:
            snprintf(send_data.buff, SENDBUFLEN, "# Dips:%d\n",dips_);
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_HISTORY:
            for(int i = 0 ; i < history_size_; i++){
                //making sure we are sending less than 1500 bytes
                if((offset < UDP_LIMIT) && ((UDP_LIMIT - offset)<6)){
                    send_data.buff_len = strlen(send_data.buff);
                    send_message(&send_data);
                    offset = 0;         //reset to 0 in order to start writing from the beginning
                    written = 0;        //reset the written bytes to 0
                }
                //writing the history to the buffer based on the offset and the limit
                written = snprintf(send_data.buff + offset, SENDBUFLEN - offset, "%.3f%s",          //writing the number of samples taken in the last second
                    history_[i], ((i+1)%10==0||i==history_size_-1)?",\n":", ");                     //if the number of samples is a multiple of 10 or the last sample then we add a new line with a comma otherwise just a comma
                offset += written;                                                                  //total bytes written so far
            }
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_STOP:
            snprintf(send_data.buff, SENDBUFLEN, "Program terminating.\n");
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_HELP:
            snprintf(send_data.buff, SENDBUFLEN,"\nAccepted command examples:\ncount -- get the total number of samples taken.\nlength -- get the number of samples taken in the previously completed second.\ndips -- get the number of dips in the previously completed second.\nhistory -- get all the samples in the previously completed second.\nstop -- cause the server program to end.\n<enter> -- repeat last command.\n");
            send_data.buff_len = strlen(send_data.buff);
            break;
        case CMD_UNKNOWN:
            snprintf(send_data.buff, SENDBUFLEN, "Unknown command. Type 'help' for command list.\n");
            send_data.buff_len = strlen(send_data.buff);
            break;
    }
}

Command_type get_current_command(void){
    Command_type ret_command;
    //critical section
    pthread_mutex_lock(&command_mutex);
    ret_command = curr_command;
    pthread_mutex_unlock(&command_mutex);
    return ret_command;
}

void update(long long a, int dip, int length, double* history, int history_size){
    total_count = a;
    length_ = length;
    dips_ = dip;
    history_ = history;
    history_size_ = history_size;
}
void stop_connection(void){
    pthread_join(communication_thread, NULL);
    //this is to make sure that the even after closing the thread, main also exits the socket
    curr_command = CMD_STOP;
    close_socket();
}

