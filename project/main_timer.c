#include "timer.h"
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>
#include "buttons.h"
#include "joystick.h"
#include "main_timer.h"

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


void main_init()
{
    
    
}
static void main_cleanup()
{
    buttons_cleanup();
    cleanup_timer(); 
    joystick_cleanup();

}

void main_timer()
{
    printf("Starting timer\n");
     
    /* start the time and run for 10 seconds */
    buttons_init();
    joystick_init();
    /* Check and display current time */
    // printf("Curent time: %d\n", currentTime());
    return;
}

void main_timer_shutdown()
{
    main_cleanup(); 
}