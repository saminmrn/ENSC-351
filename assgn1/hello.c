#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LED0 "/sys/class/leds/beaglebone:green:usr0/brightness"
#define LED1 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define LED2 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define LED3 "/sys/class/leds/beaglebone:green:usr3/brightness"

#define trigger_LED0 "/sys/class/leds/beaglebone:green:usr0/trigger"
#define trigger_LED1 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define trigger_LED2 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define trigger_LED3 "/sys/class/leds/beaglebone:green:usr3/trigger"
#define gpio_value "/sys/class/gpio/gpio72/value"

// reads the integer from the value file  of the gpio to see if the button is pressed or not (active low)
int read_int(const char *fileName)
{
    FILE *button = fopen(fileName, "r");
    int i = 0;

    fscanf(button, "%d", &i);
    while (!feof(button))
    {
        fscanf(button, "%d", &i);
    }
    fclose(button);
    return i;
}

static void runCommand(char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
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

static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

void triggerLED(char *LED, int mode)
{
    FILE *pLedTriggerFile = fopen(LED, "w");

    if (pLedTriggerFile == NULL)
    {
        printf("ERROR OPENING %s.", LED);
        exit(1);
    }
    int charWritten;
    // turning heartbeat on if mode is 1
    if (mode == 1)
    {
        charWritten = fprintf(pLedTriggerFile, "heartbeat");
    }
    // setting trigger to none if mode is 0
    else if (mode == 0)
    {
        charWritten = fprintf(pLedTriggerFile, "none");
    }

    if (charWritten <= 0)
    {
        printf("ERROR WRITING DATA");
        exit(1);
    }
    fclose(pLedTriggerFile);
}
//function to turn LEDs on or 0ff- mode 1 is on, mode 0 is off
void brightnessLED(char *LED, int mode)
{
    FILE *pLedTriggerFile = fopen(LED, "w");

    if (pLedTriggerFile == NULL)
    {
        printf("ERROR OPENING %s.", LED);
        exit(1);
    }
    int charWritten = 1;
    // turrning on LED
    if (mode == 1)
    {
        charWritten = fprintf(pLedTriggerFile, "1");
    }
    // turning of LED
    else if (mode == 0)
    {
        charWritten = fprintf(pLedTriggerFile, "0");
    }

    if (charWritten <= 0)
    {
        printf("ERROR WRITING DATA");
        exit(1);
    }
    fclose(pLedTriggerFile);
}
// light up all 4 LEDs
void light_up()
{
    brightnessLED(LED0, 1);
    brightnessLED(LED1, 1);
    brightnessLED(LED2, 1);
    brightnessLED(LED3, 1);
}
// light off all 4 LEDs
void light_off()
{
    brightnessLED(LED0, 0);
    brightnessLED(LED1, 0);
    brightnessLED(LED2, 0);
    brightnessLED(LED3, 0);
}

int main()
{
    //initializing variables
    int outer_loop = 1;
    int inner_loop = 0;
    long long start_time = 0;
    long long best_time = 5000; // initializing best time
    // printing the intro
    printf("Hello embedded world, from Samin!\n\n");
    printf("When LED3 lights up, press the USER button!\n");

    // setting the trigger to none for all LEDS
    triggerLED(trigger_LED0, 0);
    triggerLED(trigger_LED1, 0);
    triggerLED(trigger_LED2, 0);
    triggerLED(trigger_LED3, 0);
    // brightness of LED
    light_off();

    // to force the button’s pin to be treated as GPIO
    runCommand("config-pin p8.43 gpio");

    // configure the button’s pin to be input
    write_in_to_direction();

    while (outer_loop)
    {
        // reading the value of the user button
        int value = read_int(gpio_value);
        while (value == 0)//while user hold the button down, wait
        {
            value = read_int(gpio_value);
            if (value == 1)//start game when user releases the button
                break;
        }
        if (value == 1) // button unpressed
        {
            // turn off LEDs
            light_off();
            /// light up LED0
            brightnessLED(LED0, 1);
            // wait for 2s
            sleepForMs(2000);

            // If user is pressing the USER button already (too soon):
            // Record response time as 5.0s
            // Light up all LEDs
            int check = read_int(gpio_value);
            if (check == 0) // if user is already pressing the button
            {
                printf("Reaction time is 5000 ms;  ");
                printf("best so far in game is %lldms;\n", best_time);
                light_up();
                inner_loop = 0;
            }
            else // if user was not pressing the button early
            {
                // light up LED 3
                brightnessLED(LED3, 1);
                // start timer
                start_time = getTimeInMs();
                // start the game
                inner_loop = 1;
            }

            while (inner_loop)
            {
                // read the button
                int value1 = read_int(gpio_value);
                long long end_time_quit = getTimeInMs();
                // check if no response within 5s
                long long reaction_time_quit = end_time_quit - start_time;
                if (reaction_time_quit > 5000) // quit if inactive for 5s
                {
                    printf("No input within 5000 ms; quitting!\n");
                    // setting the trigger to none for all LEDS
                    triggerLED(trigger_LED0, 0);
                    triggerLED(trigger_LED1, 0);
                    triggerLED(trigger_LED2, 0);
                    triggerLED(trigger_LED3, 0);
                    // brightness of LED
                    light_off();
                    exit(0);
                }

                if (value1 == 0) // the user press the button
                {
                    // stop timer
                    long long end_time = getTimeInMs();
                    // calculate reaction time
                    long long reaction_time = end_time - start_time;

                    // new record if the reaction time is faster than the best time
                    if (reaction_time < best_time)
                    {
                        best_time = reaction_time;
                        printf("New best time!\n");
                        printf("Reaction time was  %lldms;  ", reaction_time);
                        printf("best so far in game is %lldms;\n", best_time);
                    }
                    else // no new record
                    {
                        printf("Reaction time was  %lldms;  ", reaction_time);
                        printf("best so far in game is %lldms;\n", best_time);
                    }
                    // light up all LEDs
                    light_up();
                    break;
                }
            }
        }
    }

    return 0;
}