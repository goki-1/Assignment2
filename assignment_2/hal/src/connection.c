#include "hal/connection.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

#define UDP_LIMIT 1500


pthread_t communication_thread; 
pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;

static Command_type curr_command;
static Command_type prev_command;
static bool is_first = true;

buffer_info recv_data;
buffer_info send_data;
char recv_buffer[RECVBUFLEN];
char send_buffer[SENDBUFLEN];

long long total_count = 0;
int length_ = 0, dips_ = 0, history_size_ = 0;
double* history_ = NULL;


static void lower(char *message){
    if(message == NULL) return;
    for(int i=0; message[i] != '\0'; i++){
        message[i] = tolower(message[i]);
    }
}

void* comm_func(void* data){
    buffer_info*  cach = (buffer_info*) data;
    Command_type comm = CMD_HELP;
    while(comm != CMD_STOP){
        recv_message(data);
        comm = parse_command(cach->buff);
        generate_response(comm);
        send_message(&send_data);
    }

    return NULL;
}
void start_connection(void){
    if(init_socket() == FAIL){
        perror("socket initialization failed\n");
        exit(1);
    }
    recv_data.buff = recv_buffer;
    recv_data.buff_len = RECVBUFLEN;
    send_data.buff = send_buffer;
    send_data.buff_len = SENDBUFLEN;

    pthread_mutex_init(&command_mutex, NULL);
    if(pthread_create(&communication_thread, NULL, comm_func, (void*)&recv_data) != 0){
        perror("thread creation failed\n");
        exit(1);
    }
}


Command_type parse_command(char *message){
    lower(message);
    Command_type command;
    if(*message == '\n'){
        if(is_first){
            command = CMD_HELP;
            is_first = false;
            pthread_mutex_lock(&command_mutex);
            curr_command = command;
            prev_command = curr_command;
            pthread_mutex_unlock(&command_mutex);
            return command;
        }
        else{
            pthread_mutex_lock(&command_mutex);
            command = curr_command;
            prev_command = curr_command;
            pthread_mutex_unlock(&command_mutex);
            return command;
        }
    }
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
    if(is_first){is_first = false;}
    prev_command  = curr_command;
    curr_command = command;
    return command;
}

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
                    offset = 0;
                    written = 0;
                }
                written = snprintf(send_data.buff + offset, SENDBUFLEN - offset, "%.3f%s",
                    history_[i], ((i+1)%10==0||i==history_size_-1)?",\n":", ");
                offset += written;
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
    curr_command = CMD_STOP;
    close_socket();
}

