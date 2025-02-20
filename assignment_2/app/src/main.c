#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>  // For sleep()
#include "draw_stuff.h"
#include "hal/sampler.h"

int main() {
    printf("Hello world with LCD!\n");

    // Initialize LCD and Sampler
    DrawStuff_init();
    Sampler_init();
    sleep(1);  // Give time for initialization
    char lcd_buff[128];  // Buffer for LCD
    char term_buff[128];  // Buffer for Terminal Output
    while (true) {
        

        int sample_rate = Sampler_getHistorySize();  // Number of light samples in the last second
        int flash_rate = 10;  // Fixed at 10Hz for now
        double avg_voltage = Sampler_getAverageReading();  // Average voltage (EMA)
        int dip_count = Sampler_getDipCounter();  // Number of dips detected

        snprintf(lcd_buff, sizeof(lcd_buff),
                 "Sukhman Gurkirat!\n"
                 "Flash @ %2dHz\n"
                 "Dips = %3d\n"
                 "Max ms: %.1f",
                 flash_rate, dip_count, 2.3);  

        DrawStuff_updateScreen(lcd_buff);

        snprintf(term_buff, sizeof(term_buff),
                 "#Smpl/s = %d  Flash @ %dHz  avg = %.3fV  dips = %d Smpl\n",
                 sample_rate, flash_rate, avg_voltage, dip_count);

        printf("%s\n", term_buff);

        Sampler_moveCurrentDataToHistory();

        // Sleep for 1 second before updating again
        sleep(1);
    }

    // Cleanup
    Sampler_cleanup();
    DrawStuff_cleanup();
    Sampler_cleanup();

    printf("!!! DONE !!!\n");
    return 0;
}
