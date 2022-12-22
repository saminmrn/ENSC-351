// sampling.h stores the samples and the timestamp in a 1s interval in a buffer array and 
// performs analysis on the sample values, time intervals and the number of dips 
#ifndef _SAMPLING_H_
#define _SAMPLING_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define MY_BUFFER_SIZE 1000

long long getTimeInMs(void);
void sleepForMs(long long delayInMs);
int getVoltage0Reading();

void *set_samples_array();
void *get_samples_array();

void Sampler_startSampling(void);
void Sampler_stopSampling(void);

void voltageStats(double);
void timeStats(long long, long long, int);

#endif