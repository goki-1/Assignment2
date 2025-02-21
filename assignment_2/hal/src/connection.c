#include "hal/connection.h"
//#include "circular_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

//global varaibles
pthread_t communication_thread;
//mutex to access the last and current command 
pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;
//instead of keeping track of all the commands just keep track of the last command.
//static Command_type last_command = CMD_HELP;
static Command_type curr_command;
static Command_type prev_command;
static bool is_first = true;
//buffer to store the last message received
buffer_info recv_data;
buffer_info send_data;
char recv_buffer[RECVBUFLEN];
char send_buffer[SENDBUFLEN];
//Circular_buffer* buffer;

static void lower(char *message){
    if(message == NULL) return;
    for(int i=0; message[i] != '\0'; i++){
        message[i] = tolower(message[i]);
    }
}

void* comm_func(void* data){
    buffer_info*  cach = (buffer_info*) data;
    //int s = cach->buff_len;
    //printf("%d\n",s);
    Command_type comm = CMD_HELP;
    while(comm != CMD_STOP){
        //receive the message
        recv_message(data);
        //parse the message
        comm = parse_command(cach->buff);
        //prev_command = curr_command;
        //printf("Command to pass is %d", comm);
        generate_response(comm);
        send_message(&send_data);
    }

    return NULL;
}
void start_connection(void){
    //printf("starting connection...\n");
    //initialize the socket
    if(init_socket() == FAIL){
        perror("socket initialization failed\n");
        exit(1);
    }
    //initialize the buffer data for receiver and sender
    recv_data.buff = recv_buffer;
    recv_data.buff_len = RECVBUFLEN;
    send_data.buff = send_buffer;
    send_data.buff_len = SENDBUFLEN;
    //buffer = create_circular_buffer(capacity);
    //call the thread function and then join it in the close_connection function
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
            curr_command = command;
            prev_command = curr_command;
            printf("parsed command from if is %d\n", command);
            return command;
        }
        else{
            command = curr_command;
            prev_command = curr_command;
            printf("parsed command from else is %d\n", command);
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
        else{command = CMD_HELP;}
    }
    printf("parsed command is %d\n",command);
    if(is_first){is_first = false;}
    prev_command  = curr_command;
    curr_command = command;
    return command;
}

void generate_response(Command_type command){
    //printf("The command to parse is %d\n", command);
    switch(command){
        case CMD_COUNT:
            send_data.buff = "#  samples taken total: 121212\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("0\n");
            break;
        case CMD_LENGTH:
            send_data.buff = "#  samples taken last second: 555\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("1\n");
            break;
        case CMD_DIPS:
            send_data.buff = "#  Dips: 21\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("2\n");
            break;
        case CMD_HISTORY:
            send_data.buff = "1.340, 1.340, 1.339, 1.340, 1.340, 0.977, 0.028, 0.012, 0.011, 0.012\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("3\n");
            break;
        case CMD_STOP:
            send_data.buff = "Program terminating\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("4\n");
            break;
        case CMD_HELP:
            send_data.buff = "\nAccepted command examples:\ncount -- get the total number of samples taken.\nlength -- get the number of samples taken in the previously completed second.\ndips -- get the number of dips in the previously completed second.\nhistory -- get all the samples in the previously completed second.\nstop -- cause the server program to end.\n<enter> -- repeat last command.\n";
            send_data.buff_len = strlen(send_data.buff);
            //printf("5\n");
            break;
    }
}

void stop_connection(void){
    //printf("Closing Connection...\n");
    //join the thread
    pthread_join(communication_thread, NULL);
    curr_command = CMD_STOP;
    //close the socket
    close_socket();
    //printf("Connection closed\n");
}

