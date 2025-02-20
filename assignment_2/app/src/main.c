// Main program to build the application
// Has main(); does initialization and cleanup and perhaps some basic logic.

#include <stdio.h>
#include <stdbool.h>
#include "draw_stuff.h"
#include <unistd.h>  // For sleep()

int main()
{
    printf("Hello world with LCD!\n");

    DrawStuff_init();
    sleep(1);
    //while(true){
        char buff[128];
        int flash_rate = 12;
        int dip_counter = 32;
        float max_ms = 2.3;

        snprintf(buff, 1024, 
         "Sukhman Gur!\n"
         "Flash @ %2dHz\n"
         "Dips = %3d\n"
         "Max ms: %.1f",
         flash_rate, dip_counter, max_ms);

         //snprintf(buff, 1024, "Flash @ %2dHz  Dips = %3d", flash_rate, dip_counter);


        DrawStuff_updateScreen(buff);
        //sleep(1);
        //break;
    //}
    
    // Cleanup all modules (HAL modules last)
    DrawStuff_cleanup();

    printf("!!! DONE !!!\n"); 

}