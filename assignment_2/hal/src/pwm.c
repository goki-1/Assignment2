#include "hal/pwm.h"

void pwm_write(const char *filename, int value) {
    char path[100];
    snprintf(path, sizeof(path), "%s%s", PWM_PATH, filename);
    
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        perror("Error opening PWM file");
        return;
    }
    
    fprintf(fp, "%d", value);
    fclose(fp);
}

void pwm_set_duty_cycle(int duty_cycle) {
    pwm_write("duty_cycle", duty_cycle);
}

void pwm_set_period(int period) {
    pwm_write("period", period);
}

void pwm_enable(int enable) {
    pwm_write("enable", enable);
}
