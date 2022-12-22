#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "sampling.h"
#include "joystick.h"
#include "LEDs.h"

#define gpio_value "/sys/class/gpio/gpio72/value"

// reads the integer from the value file  of the gpio to see if the button is pressed or not (active low)
int read_ints(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    int i = 0;

    fscanf(file, "%d", &i);
    while (!feof(file))
    {

        fscanf(file, "%d", &i);
    }
    fclose(file);
    return i;
}

void write_in_to_direction()
{
    // Use fopen() to open the file for write access.
    FILE *pFile = fopen("/sys/class/gpio/gpio72/direction", "w");
    if (pFile == NULL)
    {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    // Write to data to the file using fprintf():
    fprintf(pFile, "in");
    // Close the file using fclose():
    fclose(pFile);
    // Call nanosleep() to sleep for ~300ms before use.
}

int main()
{
    // to force the user button pin to be treated as GPIO and configure the button’s pin to be input
    runCommand("config-pin p8.43 gpio");
    write_in_to_direction();
    // initialize the led display
    disp_init();
    // start sampling
    Sampler_startSampling();
    while (true)
    {   
        //should hold user button for a second to shutdown
        sleepForMs(1000); 
        int ButtonNotPressed = read_ints(gpio_value);
        //active low
        if (!ButtonNotPressed)
        {
            //stop sampling and shutting down threads
            Sampler_stopSampling();
            break;
        }
    }
    return 0;
}
