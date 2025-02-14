#ifndef  ROTAR_STATEMACHINE_H_
#define  ROTAR_STATEMACHINE_H_
#include <stdbool.h>

void rotar_state_machine_init(void);
void rotar_state_machine_cleanup(void);
int rotar_state_machine_getValue(void);

void rotar_state_machine_doState();
#endif