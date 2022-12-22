#include "joystick.h"
#include "audioMixer.h"
#include "PlaySoundHandler.h"
#include "LEDs.h"
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "buttons.h"

#define A2D_FILE_VOLTAGE2 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define A2D_FILE_VOLTAGE3 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

static void *doJoystickReadingThread(void *_);
static pthread_t joystick_ID;
static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
static _Bool stopping = false;
// center direction
static int direction = 5;
static void set_direction(int d);

static void sleepForMs(long long delayInMs);
static int getVoltageReading(char);
static int getJoystickDirection(double, double);

static int getVoltageReading(char n)
{
    // Open file
    int a2dReading = 0;
    // the file for the x axis of the voltage
    if (n == 'x' || n == 'X')
    {
        FILE *f = fopen(A2D_FILE_VOLTAGE2, "r");
        if (!f)
        {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);
        if (itemsRead <= 0)
        {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
    }
    else if (n == 'Y' || n == 'y') // the file for the y axis of the voltage
    {
        FILE *f = fopen(A2D_FILE_VOLTAGE3, "r");
        if (!f)
        {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);
        if (itemsRead <= 0)
        {
            printf("ERROR: Unable to read values from voltage input file.\n");
            exit(-1);
        }
        // Close file
        fclose(f);
    }

    return a2dReading;
}

static int getJoystickDirection(double x, double y)
{
    if (x < 0.45)
    {
        // UP
        return 1;
    }
    else if (x > 1.35)
    {
        // DOWN
        return 2;
    }
    else if (y < 0.45)
    {
        // LEFT
        return 4;
    }
    else if (y > 1.35)
    {
        // RIGHT
        return 3;
    }
    else
    {
        // CENTER
        return 5;
    }
}

static void *doJoystickReadingThread(void *_)
{

    while (!stopping)
    {
        double voltage_x = ((double)getVoltageReading('x') / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        double voltage_y = ((double)getVoltageReading('y') / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
        int dir = getJoystickDirection(voltage_x, voltage_y);
        if (dir == 1)
        {
            // printf("UP\n");
            increaseVolume();
            Display_number(getNewVolume());
            sleepForMs(1000);
        }
        else if (dir == 2)
        {
            // printf("DOWN\n");
            decreaseVolume();
            Display_number(getNewVolume());
            sleepForMs(1000);
        }
        else if (dir == 3)
        {
            // printf("LEFT\n");
            decreaseTempo();
            Display_number_double((double)getNewTempo() / 60);
            sleepForMs(1000);
        }
        else if (dir == 4)
        {
            // printf("RIGHT\n");
            increaseTempo();
            Display_number_double((double)getNewTempo() / 60);
            sleepForMs(1000);
        }
        else if (dir == 5)
        {
            Display_mode(get_mode());
        }
    }
}

void joystick_init()
{
    pthread_create(&joystick_ID, NULL, doJoystickReadingThread, NULL);
}

void joystick_cleanup()
{
    stopping = true;
    pthread_join(joystick_ID, NULL);
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