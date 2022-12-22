#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "buttons.h"
#include "BeatBox.h"
#include "LEDs.h"
#include "joystick.h"

#define GREYBUTTON_Direction "/sys/class/gpio/gpio47/direction"   // p8.15
#define REDBUTTON_Direction "/sys/class/gpio/gpio46/direction"    // p8.16
#define YELLOWBUTTON_Direction "/sys/class/gpio/gpio27/direction" // p8.17
#define GREENBUTTON_Direction "/sys/class/gpio/gpio65/direction"  // p8.18

#define GREYBUTTON_Value "/sys/class/gpio/gpio47/value"   // p8.15
#define REDBUTTON_Value "/sys/class/gpio/gpio46/value"    // p8.16
#define YELLOWBUTTON_Value "/sys/class/gpio/gpio27/value" // p8.17
#define GREENBUTTON_Value "/sys/class/gpio/gpio65/value"  // p8.18

static int current_status = 0;
static void set_button_status(int status);
static int get_button_status();
static int current_mode = 1;
static void set_mode(int mode);

static int read_ints(const char *fileName);
static void write_in_to_direction(const char *fileName);
static void runCommand(char *command);
static _Bool stopping = false;

// thread for the button
static void *get_button_status_thread(void *_);
static pthread_t get_button_status_thread_ID;
static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;

static void sleepForMs(long long delayInMs);
static long long getTimeInMs(void);

// reads the integer from the value file  of the gpio to see if the button is pressed or not (active low)
static int read_ints(const char *fileName)
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

static void write_in_to_direction(const char *fileName)
{
    // Use fopen() to open the file for write access.
    FILE *pFile = fopen(fileName, "w");
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

void buttons_init()
{
    // add export for the gpio
    // configure button pins as gpio
    runCommand("config-pin p8.15 gpio");
    runCommand("config-pin p8.16 gpio");
    runCommand("config-pin p8.17 gpio");
    runCommand("config-pin p8.18 gpio");
    // write button pins direction to in
    write_in_to_direction(GREYBUTTON_Direction);
    write_in_to_direction(REDBUTTON_Direction);
    write_in_to_direction(YELLOWBUTTON_Direction);
    write_in_to_direction(GREENBUTTON_Direction);

    pthread_create(&get_button_status_thread_ID, NULL, get_button_status_thread, NULL);
}

void *get_button_status_thread(void *_)
{
    // get button pins value (active low)
    int greyPressed = 0;
    int redPressed = 0;
    int yellowPressed = 0;
    int greenPressed = 0;

    printf("Scanning for buttons....\n\n");

    while (!stopping)
    {
        greyPressed = read_ints(GREYBUTTON_Value);
        redPressed = read_ints(REDBUTTON_Value);
        yellowPressed = read_ints(YELLOWBUTTON_Value);
        greenPressed = read_ints(GREENBUTTON_Value);
        // play sound upon button release
        if (greyPressed)
        {
            if (get_button_status() != 1)
            {
                set_button_status(1);
                current_mode++;
            }
        }
        else if (redPressed)
        {
            if (get_button_status() != 1)
            {
                set_button_status(1);
                oneBeat(1);
            }
        }
        else if (yellowPressed)
        {
            if (get_button_status() != 1)
            {
                set_button_status(1);
                oneBeat(2);
            }
        }
        else if (greenPressed)
        {
            if (get_button_status() != 1)
            {
                set_button_status(1);
                oneBeat(3);
            }
        }
        else
        {
            if (get_button_status() != 0)
            {
                set_button_status(0);
            }
        }
        sleepForMs(100);
    }
}

static void set_button_status(int status)
{
    current_status = status;
}

static int get_button_status()
{
    return current_status;
}

int get_mode()
{
    if (current_mode % 3 == 1)
    {
        return 1;
    }
    if (current_mode % 3 == 2)
    {
        return 2;
    }
    if (current_mode % 3 == 0)
    {
        return 0;
    }
}

// changing he mode when grey button pressed
static void set_mode(int mode)
{
    current_mode = mode;
}

void buttons_cleanup()
{
    stopping = true;
    pthread_join(get_button_status_thread_ID, NULL);
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
