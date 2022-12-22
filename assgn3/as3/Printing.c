#include "Printing.h"
#include "intervalTimer.h"
#include "buttons.h"
#include "audioMixer.h"
#include "BeatBox.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

static _Bool stopping = false;

// thread for the button
static void *printing_thread(void *_);
static pthread_t printing_thread_ID;
static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;

// enum Interval_whichInterval interval;
// Interval_statistics_t *p;

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

void *printing_thread(void *_)
{

    while (!stopping)
    {
        setStats();
        setStatsBB();
        // M1 120bpm vol:80 Low[20.83, 22.11] avg 21.32/47 Beat[250.13, 250.53] avg 250.25/4
        printf("M%d  %dbpm %dvol Low[%f, %f] avg: %f/%d Beat[{%f, %f] avg: %f/%d\n",
               get_mode(), AudioMixer_get_BPM(), AudioMixer_getVolume(), getMin(), getMax(), getAverage(), getSamples(), getMinBB(), getMaxBB(), getAverageBB(), getSamplesBB());
        sleepForMs(1000);
    }
}

void printing_init()
{
    pthread_create(&printing_thread_ID, NULL, printing_thread, NULL);
}

void printing_cleanup()
{
    stopping = true;
    pthread_join(printing_thread_ID, NULL);
}