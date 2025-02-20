#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// Generic node for the circular buffer
typedef struct{
    void* data;
} Circular_buffer_entry;


typedef struct{
    Circular_buffer_entry* data_struct;
    int head, tail, size;
    long capacity;
} Circular_buffer;

// Functions to work on the circular buffer
//constructor and destructor
Circular_buffer* create_circular_buffer(long capacity);
void destroy_circular_buffer(Circular_buffer* buffer);

//operations
bool is_full(Circular_buffer* buffer);
bool is_empty(Circular_buffer* buffer);
bool insert(Circular_buffer* buffer, void* data);
//for getting the tail and the previous tail element for using in commands
void* get_tail_element(Circular_buffer* buffer);
void* get_prev_tail_element(Circular_buffer* buffer);
//for printing the buffer
void print_buffer(Circular_buffer* buffer);

#endif