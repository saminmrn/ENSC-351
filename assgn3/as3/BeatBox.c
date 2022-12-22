#include "BeatBox.h"
#include "joystick.h"
#include "audioMixer.h"
#include "buttons.h"
#include "intervalTimer.h"
#include "LEDs.h"
#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#define SOURCE_FILE_DRUM "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define SOURCE_FILE_SNARE "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"
#define SOURCE_FILE_HIHAT "beatbox-wav-files/100053__menegass__gui-drum-cc.wav"

static void RockDrumBeat();
static void noDrumBeat();
static void CustomDrumBeat();
static void rock();
void oneBeat(int i);

static int count = 0;

enum Interval_whichInterval beatboxInterval = INTERVAL_BEAT_BOX;
Interval_statistics_t pBeat;

static void *play_beat_thread(void *_);
static pthread_t play_beat_thread_ID;
static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
static _Bool stopping = false;

static void sleepForMs(long long delayInMs);

static wavedata_t drum;
static wavedata_t snare;
static wavedata_t hihat;

// standard rock beat
static void RockDrumBeat()
{
    double delayTime = wait_for_halfBeat();

    AudioMixer_queueSound(&hihat);
    AudioMixer_queueSound(&drum);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&hihat);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&hihat);
    AudioMixer_queueSound(&snare);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&hihat);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);
}
static void noDrumBeat()
{
    return;
}
static void CustomDrumBeat()
{
    double delayTime = wait_for_halfBeat();

    AudioMixer_queueSound(&drum);
    AudioMixer_queueSound(&snare);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&drum);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&drum);
    AudioMixer_queueSound(&snare);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);

    AudioMixer_queueSound(&snare);
    sleepForMs(delayTime);
    Interval_markInterval(beatboxInterval);
}

// called from where the modes chage
// play one beat at a time
void oneBeat(int i)
{
    double delayTime = wait_for_halfBeat();
    // Base Drum
    if (i == 1)
    {
        AudioMixer_queueSound(&drum);
    }
    // Snare
    if (i == 2)
    {
        AudioMixer_queueSound(&snare);
    }
    // HiHat
    if (i == 3)
    {
        AudioMixer_queueSound(&hihat);
    }
}
// 2 sequences of the rock beat
static void rock()
{
    RockDrumBeat();
}

static void *play_beat_thread(void *_)
{
    while (!stopping)
    {
        int mode = get_mode();
        if (mode == 1)
        {
            rock();
        }
        else if (mode == 2)
        {
            CustomDrumBeat();
        }
        else if (mode == 0)
        {
            noDrumBeat();
        }
    }
}

void BeatBox_init()
{
    AudioMixer_readWaveFileIntoMemory(SOURCE_FILE_DRUM, &drum);
    AudioMixer_readWaveFileIntoMemory(SOURCE_FILE_SNARE, &snare);
    AudioMixer_readWaveFileIntoMemory(SOURCE_FILE_HIHAT, &hihat);
    pthread_create(&play_beat_thread_ID, NULL, play_beat_thread, NULL);
}

void BeatBox_cleanup()
{
    stopping = true;
    pthread_join(play_beat_thread_ID, NULL);
    AudioMixer_freeWaveFileData(&snare);
    AudioMixer_freeWaveFileData(&hihat);
    AudioMixer_freeWaveFileData(&drum);
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

void set_count(int c)
{
    count = c;
}

void setStatsBB()
{
    Interval_getStatisticsAndClear(beatboxInterval, &pBeat);
}

int getSamplesBB()
{
    return pBeat.numSamples;
}

double getAverageBB()
{
    return pBeat.avgIntervalInMs;
}

double getMinBB()
{
    return pBeat.minIntervalInMs;
}

double getMaxBB()
{
    return pBeat.maxIntervalInMs;
}