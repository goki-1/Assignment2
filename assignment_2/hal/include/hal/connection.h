#ifndef CONNECTION_H
#define CONNECTION_H
/*
* This file runs the connection between main and udp
* It will start the connection, parse the command, generate the response, stop the connection, and update the values.
* It will also have a function to get the current command that is being requested.
*/
#include "udp.h"
#include <pthread.h>

/**
 *  Enum representing different command types.
 */
typedef enum {
    CMD_HELP,      ///< Help command (`help` or `?`)
    CMD_COUNT,     ///< Count command (`count`)
    CMD_LENGTH,    ///< Length command (`length`)
    CMD_DIPS,      ///< Dips command (`dips`)
    CMD_HISTORY,   ///< History command (`history`)
    CMD_STOP,      ///< Stop command (`stop`)
    CMD_UNKNOWN    ///< unknown command
} Command_type;

/**
 * Starts the UDP connection and begins listening for messages.
 * 
 * This function initializes the socket and launches a receiver thread.
 */
void start_connection(void);


/**
 * Converts a message string to a command type.
 * 
 * Parses the received message and returns the corresponding `CommandType` enum.
 * If the message is unrecognized, returns `CMD_UNKNOWN`.
 * 
 * message The received command string.
 * Corresponding `CommandType` value.
 */
Command_type parse_command(char *message);

/**
 * Generates a response based on the given command.
 * 
 * Creates and returns a `buffer_info` structure containing the response message.
 * The caller is responsible for freeing the allocated memory.
 * 
 * The command type to generate a response for.
 * Pointer to a dynamically allocated `buffer_info` containing the response.
 */
void generate_response(Command_type command);

/**
 *  Function to be used inside the receiver thread.
 * 
 *  This function will call the recv_msg function and then send the response.
 * 
 *  data The thread data structure containing the socket and the buffer.
 * 
 *  no return type as it is used in a thread.
 */
void* comm_func(void* data);



/**
 * Stops the UDP connection and closes the socket.
 * 
 * This function stops the receiver thread and closes the socket.
 */
void stop_connection(void);

/**
 *  Function to be used inside the main.
 * 
 *  This function will read values from main in order to send the data requested in response.
 * 
 * a is for total samples taken so far , length is for the length of samples taken in last second, history is the pointer to the array of samokes and history_size is the size of the array.
 *  
 *  no return type as it is an update funciton
 */ 
void update(long long a, int dip, int length, double* history, int history_size);

/**
 *  Function to be used inside main.
 * 
 *  This function is used to get the current command that is being requested.
 * 
 *  when asked to stop, it will return CDM_STOP, which is used to initiate the closing down of the main
 * 
 *  return type is enum Command_type described on the top of the file.
 */
Command_type get_current_command(void);
#endif