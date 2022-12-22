#include "sampling.h"
#include "joystick.h"
#include "LEDs.h"

typedef struct
{
    double voltage_reading;
    long long time_stamp;
} samples;

samples mybuffer[MY_BUFFER_SIZE];
int mybuffer_index = 0;
long long lastTime = 0;
double firstVoltage = 0;
//voltage variables
double average_Voltage = 0;
double maximum_Voltage = 0;
double minimum_Voltage = 1000000000000;
int dips = 0;
//time variables 
long double average_Time = 0;
long double totalTime = 0;
long long maximum_Time = 0;
long long minimum_Time = 10000000000;
//threads
static pthread_t idSet, idGet;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool isRunning = true;
bool analysisReady = false;
bool isDipped = false;
bool firstTime = true;

long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000 + nanoSeconds / 1000000;
    return milliSeconds;
}
//updated the milisecond to sleep in nano second
long long getTimeInNs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long nanoSecondsTime = seconds * 1000000000 + nanoSeconds;
    return nanoSecondsTime;
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *)NULL);
}

int getVoltage0Reading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!f)
    {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0)
    {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}

void *set_samples_array()
{
    printf("Starting sampling thread\n");
    while (isRunning)
    {
        analysisReady = false;
        mybuffer_index = 0;
        long double StartTime = getTimeInMs();
        pthread_mutex_lock(&mutex);
        while ((StartTime + 1000) >= getTimeInMs())
        {
            int reading = getVoltage0Reading();
            double voltage = ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
            long long time = getTimeInNs();
            //storing values in the buffer
            mybuffer[mybuffer_index].time_stamp = time;
            mybuffer[mybuffer_index].voltage_reading = voltage;
            sleepForMs(1);
            mybuffer_index++;
        }
        if (firstTime)//first time running 
        {
            lastTime = mybuffer[mybuffer_index - 1].time_stamp;
            firstTime = false;
        }
        mybuffer_index--;
        firstVoltage = mybuffer[0].voltage_reading;
        pthread_mutex_unlock(&mutex);
        analysisReady = true;
        sleepForMs(0);
    }
    printf("Stopping sampling thread\n");
    return NULL;
}

void *get_samples_array()
{
    sleep(2);
    while (isRunning)
    {
        if (analysisReady)
        {
            pthread_mutex_lock(&mutex);
            int sampleNum = mybuffer_index;
            long double time = lastTime - 1000000000;
            double voltage = firstVoltage;
            samples mybuffer_copy[sampleNum];
            for (int i = 0; i < sampleNum; i++)
            {
                mybuffer_copy[i].time_stamp = mybuffer[i].time_stamp;
                mybuffer_copy[i].voltage_reading = mybuffer[i].voltage_reading;
            }
            pthread_mutex_unlock(&mutex);
            maximum_Voltage = 0;
            minimum_Voltage = 1000000000000;
            maximum_Time = 0;
            minimum_Time = 1000000000000;
            average_Time = 0;
            totalTime = 0;
            voltageStats(voltage);
            for (int i = 0; i < sampleNum; i++)
            {
                voltageStats(mybuffer_copy[i].voltage_reading);
                timeStats(mybuffer_copy[i].time_stamp, time, i);
                time = mybuffer_copy[i].time_stamp;
            }
            lastTime = time;
            double voltage_x = ((double)getVoltageReading('x') / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
            double voltage_y = ((double)getVoltageReading('y') / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
            long double minimum_Time_double = (long double)minimum_Time / 1000000;
            long double maximum_Time_double = (long double)maximum_Time / 1000000;
            int dir = getJoystickDirection(voltage_x, voltage_y);
            if (dir == 1)
            {
                // printf("UP\n");
                Display_number_double(maximum_Voltage);
            }
            else if (dir == 2)
            {
                // printf("DOWN\n");
                Display_number_double(minimum_Voltage);
            }
            else if (dir == 3)
            {
                // printf("LEFT\n");
                Display_number_double(minimum_Time_double);
            }
            else if (dir == 4)
            {
                // printf("RIGHT\n");
                Display_number_double(maximum_Time_double);
            }
            else if (dir == 5)
            {
                // printf("CENTER\n");
                Display_number(dips);
            }
            else
            {
                printf("NONE\n");
            }
            // Print data in the format: Interval ms (0.000, 3.058) avg=1.825   Samples V (1.289, 1.300) avg=1.124   # Dips:   0   # Samples:    547
            printf("Interval ms (%Lf, %Lf) avg=%Lf   Samples V (%f, %f) avg=%f   # Dips:   %3d   # Samples:    %d\n", minimum_Time_double, maximum_Time_double, average_Time, minimum_Voltage, maximum_Voltage, average_Voltage, dips, sampleNum);
            analysisReady = true;
            dips = 0;
        }
    }
    return NULL;
}

void Sampler_startSampling(void)
{
    isRunning = true;
    pthread_create(&idSet, NULL, &set_samples_array, NULL);
    pthread_create(&idGet, NULL, &get_samples_array, NULL);
}
void Sampler_stopSampling(void)
{
    isRunning = false;
    pthread_join(idSet, NULL);
    pthread_join(idGet, NULL);
    printf("Shutting down!\n");
}

void voltageStats(double Voltage)
{
    // check if the voltage is below the average voltage
    if ((Voltage <= (average_Voltage + 0.1)))
    {
        if (!isDipped)
        {
            dips++;
            isDipped = true;
        }
    }
    if ((Voltage > (average_Voltage + 0.13)))
    {
        isDipped = false;
    }
    //average weighing the previous average 99.9%
    average_Voltage = (0.001 * Voltage) + (0.999 * average_Voltage);
    if (Voltage > maximum_Voltage)
    {
        maximum_Voltage = Voltage;
    }
    if (Voltage < minimum_Voltage)
    {
        minimum_Voltage = Voltage;
    }
}

void timeStats(long long Time, long long prevTime, int size)
{
    long long timeDifference = Time - prevTime;
    //first element in array
    if (size == 0)
    {
        timeDifference = timeDifference - 1000000000;
    }
    int newSize = size + 1;
    //finding average time 
    totalTime = totalTime + ((long double) timeDifference / 1000000);
    average_Time = totalTime / newSize;
    //updating the time min and max 
    if (timeDifference > maximum_Time)
    {
        maximum_Time = (timeDifference );
    }
    if (timeDifference < minimum_Time && timeDifference > 0)
    {
        minimum_Time = timeDifference;
    }
}