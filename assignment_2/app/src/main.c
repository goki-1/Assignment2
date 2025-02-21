#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>  // For sleep()
#include "draw_stuff.h"
#include "hal/sampler.h"
#include "hal/rotar_statemachine.h"
#include "hal/gpio.h"
#include "hal/pwm.h"
#include "hal/periodTimer.h"


int main() {
    //printf("Hello world with LCD!\n");

    // Initialize LCD and Sampler
    Period_init();
    Gpio_initialize();
    DrawStuff_init();
    Sampler_init();
    init_pwm();
    rotar_state_machine_init();

    sleep(1);  // Give time for initialization
    char lcd_buff[128];  // Buffer for LCD
    char term_buff[128];  // Buffer for Terminal Output
    Period_statistics_t periodStats;

    while (true) {
        //int value = rotar_state_machine_get_value();
        int sample_rate = Sampler_getHistorySize();  // Number of light samples in the last second
        int flash_rate = rotar_state_machine_get_value(); 

        change_frequency(flash_rate);

        double avg_voltage = Sampler_getAverageReading();  // Average voltage (EMA)
        int dip_count = Sampler_getDipCounter();  // Number of dips detected

        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &periodStats);

        snprintf(lcd_buff, sizeof(lcd_buff),
                 "Sukhman Gurkirat!\n"
                 "Flash @ %2dHz\n"
                 "Dips = %3d\n"
                 "Max ms: %.1f",
                 flash_rate, dip_count, periodStats.maxPeriodInMs);  

        DrawStuff_updateScreen(lcd_buff);

        snprintf(term_buff, sizeof(term_buff),
                 "#Smpl/s = %d  Flash @ %dHz  avg = %.3fV  dips = %d  "
                 "Smpl ms [%.3f, %.3f]  avg = %.3f/%d\n",
                 sample_rate, flash_rate, avg_voltage, dip_count,
                 periodStats.minPeriodInMs, periodStats.maxPeriodInMs,
                 periodStats.avgPeriodInMs, sample_rate);

        printf("%s\n", term_buff);

        Sampler_moveCurrentDataToHistory();
        // Sleep for 1 second before updating again
        sleep(1);
        //value = rotar_state_machine_get_value();
    }

    // Cleanup
    Sampler_cleanup();
    DrawStuff_cleanup();
    Sampler_cleanup();
    rotar_state_machine_cleanup();
    Gpio_cleanup();
    close_pwm();
    Period_cleanup();

    printf("!!! DONE !!!\n");
    return 0;
}
