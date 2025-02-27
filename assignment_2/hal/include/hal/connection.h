#ifndef CONNECTION_H
#define CONNECTION_H

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

void update(long long a, int dip, int length, double* history, int history_size);

Command_type get_current_command(void);
#endif