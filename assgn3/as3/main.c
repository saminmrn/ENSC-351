#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <ctype.h>
#include "joystick.h"
#include "LEDs.h"
#include "buttons.h"
#include "audioMixer.h"
#include "BeatBox.h"
#include "intervalTimer.h"
#include "Printing.h"
#include <pthread.h>

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

// starting threads
void main_init()
{
    disp_init();
    buttons_init();
    BeatBox_init();
    Interval_init();
    AudioMixer_init();
    joystick_init();
    printing_init();
}
// shutting down threads
void main_cleanup()
{
    buttons_cleanup();
    AudioMixer_cleanup();
    BeatBox_cleanup();
    joystick_cleanup();
    printing_cleanup();
    Interval_cleanup();
}

void *main_quit(void *arg)
{
    while (true)
    {
        sleepForMs(100);
        if (toupper(getchar()) == 'Q')
        {
            main_cleanup();
            break;
        }
    }
    return NULL;
}

int main()
{
    main_init();
    printf("Enter 'Q' to quit.\n");
    pthread_t quitThread;
    pthread_create(&quitThread, NULL, main_quit, NULL);
    pthread_join(quitThread, NULL);
    return 0;
}
