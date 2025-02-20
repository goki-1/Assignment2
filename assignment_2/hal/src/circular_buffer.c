#include "circular_buffer.h"
#include <assert.h>
static bool is_initialized = false;

Circular_buffer* create_circular_buffer(long capacity){
    assert(!is_initialized);
    if(capacity < 1){
        printf("Capacity must be atleast 1\n");
        return NULL;
    }
    //this is the actual buffer that will be returned
    Circular_buffer* buffer = (Circular_buffer*)malloc(sizeof(Circular_buffer));
    if(buffer == NULL){
        printf("Memory allocation failed\n");
        return NULL;
    }
    //this is the actual data structure that will hold the data
    buffer->data_struct = (Circular_buffer_entry*)malloc(capacity * sizeof(Circular_buffer_entry));

    if(buffer->data_struct == NULL){
        printf("Memory allocation failed\n");
        free(buffer);
        return NULL;
    }
    buffer->capacity = capacity;
    buffer->size = 0;
    buffer->head = 0;
    buffer->tail = 0;
    is_initialized = true;
    //returns the pointer to the buffer
    printf("DEBUG buffer_initialized\n");
    return buffer; 
}

void destroy_circular_buffer(Circular_buffer* buffer){
    if(buffer == NULL) return;
    assert(is_initialized);
    printf("DEBUG buffer_destroyed\n");
    free(buffer->data_struct);
    free(buffer);
}

bool is_full(Circular_buffer* buffer){
    assert(is_initialized);
    return buffer->size == buffer->capacity;
}

bool is_empty(Circular_buffer* buffer){
    assert(is_initialized);
    return buffer->size == 0;
}


bool insert(Circular_buffer* buffer, void* data){
    if (is_full(buffer)) {
        // Overwrite the oldest element by moving head
        buffer->head = (buffer->head + 1) % buffer->capacity;   
    } else {
        buffer->size++;  // Only increase size if not already full
    }

    buffer->data_struct[buffer->tail].data = data;
    buffer->tail = (buffer->tail + 1) % buffer->capacity;

    return true;
}

void* get_tail_element(Circular_buffer* buffer){
    assert(is_initialized);
    if(is_empty(buffer)){
        printf("Buffer is empty\n");
        return NULL;
    }
    return buffer->data_struct[(buffer->tail - 1)].data;
}

void* get_prev_tail_element(Circular_buffer* buffer){
    assert(is_initialized);
    if(is_empty(buffer)){
        printf("Buffer is empty\n");
        return NULL;
    }
    return buffer->data_struct[(buffer->tail - 2 + buffer->capacity) % buffer->capacity].data;
}

void print_buffer(Circular_buffer* buffer) {
    if (is_empty(buffer)) {
        printf("Buffer is empty.\n");
        return;
    }
    
    printf("Current buffer state:\n");
    int index = buffer->head;
    for (int i = 0; i < buffer->size; i++) {
        printf("%d ", *(int*)buffer->data_struct[index].data);
        index = (index + 1) % buffer->capacity;
    }
    printf("\n");
}

