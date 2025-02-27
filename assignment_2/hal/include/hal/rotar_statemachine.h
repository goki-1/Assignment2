#ifndef  ROTAR_STATEMACHINE_H_
#define  ROTAR_STATEMACHINE_H_
#include <stdbool.h>

void rotar_state_machine_init(void);
void rotar_state_machine_cleanup(void);
int rotar_state_machine_get_value(void);

void rotar_state_machine_do_state();
void set_counter(int value);
#endif