#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PWM_PATH "/dev/hat/pwm/GPIO12/"
void int_pwn();
void pwm_set_duty_cycle(int duty_cycle);
void pwm_set_period(int period);
void pwm_enable(int enable);


#endif
