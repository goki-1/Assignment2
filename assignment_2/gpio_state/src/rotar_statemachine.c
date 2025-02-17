#include "rotar_statemachine.h"
#include "gpio.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>


#define GPIO_CHIP   GPIO_CHIP_2
#define GPIO_LINE_A  7
#define GPIO_LINE_B  8

//global variables
static bool is_initialized = false;
//line A and B
static struct GpioLine* s_lineA = NULL;
static struct GpioLine* s_lineB = NULL;
//atomic variable to store the counter
static atomic_int counter = 0;

struct stateEvent {
    struct state* pNextState;
    void (*action)();
};

struct state {
    struct stateEvent a_rising;
    struct stateEvent a_falling;
    struct stateEvent b_rising;
    struct stateEvent b_falling;
};
//helper functions for the state machine
static void rotary_clockwise(void){
    counter++;
}
static void rotary_counter_clockwise(void){
    counter--;
}
//STATE MACHINE DESCRIPTION
struct state states[] = {
    {// 0
        .a_rising  = {&states[0], NULL},                        //on A rising stay in 0
        .a_falling = {&states[1], NULL},                        //on A falling move from 0->1
        .b_rising  = {&states[0], NULL},                        //on B rising stay in 0 
        .b_falling = {&states[3], NULL},                        //on B falling move from 0->3
    },

    {// 1
        .a_rising  = {&states[0], rotary_counter_clockwise},    //on A rising move from 1->REST and counter - 1  
        .a_falling = {&states[1], NULL},                        //on A falling stay in 1
        .b_rising  = {&states[1], NULL},                        //on B rising stay in 1
        .b_falling = {&states[2], NULL},                        //on B falling move from 1->2   
    },
    {// 2
        .a_rising  = {&states[3], NULL},                        //on A rising move from 2->3
        .a_falling = {&states[2], NULL},                        //on A falling stay in 2
        .b_rising  = {&states[1], NULL},                        //on B rising move from 2->1    
        .b_falling = {&states[2], NULL},                        //on B falling stay in 2
    },
    {// 3
        .a_rising  = {&states[3], NULL},                        //on A rising stay in 3 
        .a_falling = {&states[2], NULL},                        //on A falling move from 3->2   
        .b_rising  = {&states[0], rotary_clockwise},            //on B rising move from 3->REST and counter + 1
        .b_falling = {&states[3], NULL},                        //on B falling stay in 3
    },
};

//this is the beginning state of the state machine
struct state* pCurrentState = &states[0];

void rotar_state_machine_init(){
    assert(!is_initialized);
    s_lineA = Gpio_openForEvents(GPIO_CHIP, GPIO_LINE_A);
    s_lineB = Gpio_openForEvents(GPIO_CHIP, GPIO_LINE_B);
    is_initialized = true;
}

void rotar_state_machine_cleanup(){
    assert(is_initialized);
    is_initialized = false;
    Gpio_close(s_lineA);
    Gpio_close(s_lineB);
}

int rotar_state_machine_get_value(){
    return counter;
}

//this function needs to be in a background thread therefre after the init function start this as a thread.
void rotar_state_machine_do_state(){
    assert(is_initialized);
    struct gpiod_line_bulk bulkEvents;
    int num_events = Gpio_waitForLineChange(s_lineA, s_lineB, &bulkEvents);
    for(int i = 0 ;i < num_events ; i++){
        //Get the line handle for this event
        struct gpiod_line *line_handle = gpiod_line_bulk_get_line(&bulkEvents, i);

        //Ge tht number of this line
        unsigned int this_line_number = gpiod_line_offset(line_handle);

        //Get the line event
        struct gpiod_line_event event;
        if(gpiod_line_event_read(line_handle, &event) == -1){
            perror("Line Event");
            exit(EXIT_FAILURE);
        }

        //Run the state machine
        bool is_rising = (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE);
        //check for which line number the event is triggered
        bool is_A = (this_line_number == GPIO_LINE_A);
        bool is_B = (this_line_number == GPIO_LINE_B);

        assert(is_A || is_B);

        struct stateEvent* pEvent = NULL;
        if(is_A){// Process A rising or falling
                if(is_rising){
                    pEvent = &pCurrentState->a_rising;
                }
                else{
                    pEvent = &pCurrentState->a_falling;
                }    
        } 
        else if(is_B) {// Process B rising or falling
                if(is_rising){
                    pEvent = &pCurrentState->b_rising;
                }
                else{
                    pEvent = &pCurrentState->b_falling;
                }
        }
        // Execute the action if any
        if(pEvent->action != NULL) {
            //this is where we call the counter++ or counter -- if the action is not NULL
            pEvent->action();  
        }
        //change the current state to the next state
        pCurrentState = pEvent->pNextState;
    }
}