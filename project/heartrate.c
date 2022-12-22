#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
// #include "LEDs.h"

static void *heartRateThread(void *_);
static pthread_t heartRateThread_ID;
// static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
static _Bool stopping = false;

static long long getTimeInMs(void);
static void sleepForMs(long long delayInMs);
// static void write_in_to_direction();
static int read_int(const char *fileName);
int bpm = 0;

#define gpio_value "/sys/class/gpio/gpio50/value"

static void write_in_to_direction()
{
    // Use fopen() to open the file for write access.
    FILE *pFile = fopen("/sys/class/gpio/gpio50/direction", "w");
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

int getHeartRate() {
    return bpm;
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

static int read_int(const char *fileName)
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

static void *heartRateThread(void *_)
{
    int count = 0;
    bool countSatus = 0;
    long long starttime = getTimeInMs();
    while (!stopping) // Reading pulse sensor for 10 seconds
    {
        int value = read_int(gpio_value);

        if (countSatus == 0)
        {
            // clear();
            value = read_int(gpio_value);
            if (value == 1)
            {
                // printf("Beat Detected \n");
                // display_heart();
                count++;
                countSatus = 1;
                // printf("Beat: beat \n");
            }
        }
        else
        {
            if (value == 0)
            {
                countSatus = 0;
            }
        }
        if (getTimeInMs() - starttime > 15000)
        {
            bpm = count * 4;
            if (bpm <= 99)
            {
                // Display_number(bpm);
                sleepForMs(1500); 
            }

            count = 0;
            countSatus = 0;
            // printf("BPM: %d \n", bpm);
            starttime = getTimeInMs();
        }
        sleepForMs(10);
    }
    return NULL;
}

void start_heartrate()
{
    // write_in_to_direction();
    pthread_create(&heartRateThread_ID, NULL, heartRateThread, NULL);
}

void cleanup_heartrate()
{
    stopping = true;
    pthread_join(heartRateThread_ID, NULL);
}