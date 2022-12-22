// timer.c 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include "timer.h"
#include "oled.h"
 
/* prototypes */
static void * start_timer_thread(void *_);
static pthread_t timer_thread_id;
static clock_t current_time = 0;
static int time_in_seconds=0; 
static int time_in_minutes=0; 

bool cancelled= false ; 
bool stopping_timer =false; 

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

void set_timer_seconds(int new_timer)
{
    if (new_timer >=0 && new_timer<60)
    {
        time_in_seconds= new_timer; 
    }
    else if (new_timer>=60)//cannot go above 60 for seconds
    {
        time_in_seconds=0; 
    }
    
}
void set_timer_minutes(int new_timer)
{
    if (new_timer>=0 )
    {
        time_in_minutes= new_timer; 
    }
    
}
int get_timer_seconds()
{
    return time_in_seconds; 
}

int get_timer_minutes()
{
    return time_in_minutes; 
}

/* create the thread */
void start_timer()
{
    stopping_timer=false; 
    printf("Starting thread\n");
    pthread_create(&timer_thread_id, NULL, start_timer_thread, (void*) time_in_seconds);
}

/* start the timing in another thread */
static void* start_timer_thread(void *_)
{
    cancelled = false; 
    while (!stopping_timer)
    {
        printf("Starting thread\n");
        //int seconds = (int) secs;
        printf("start_timer: %d minutes and %d seconds\n",(int)time_in_minutes,  (int) time_in_seconds);
    
        current_time = clock() + (time_in_minutes*CLOCKS_PER_SEC *60 + time_in_seconds * CLOCKS_PER_SEC);
        
        /* loop until the 10 seconds has reached */
        while(clock() < current_time){
            if (cancelled)
            {
                
                sleep(1);
                break; 
            }
        }
        printf("Done timer\n");
        displayWorkoutComplete();
        // sleep(5);
        //play the congrats sound
        // if (!cancelled)
        // {
        //     runCommand("aplay congrats.wav");
        // } 
        stopping_timer=true; 
        cleanup_timer();   
    }
    return NULL; 
}
 
/* get the current time of work */
int currentTime()
{
    return (int) current_time / CLOCKS_PER_SEC;
}

void cleanup_timer()
{
    //printf("Shutting down thread\n");
    stopping_timer = true; 
    pthread_join(timer_thread_id, NULL); 
}
void cancel_timer()
{
    stopping_timer= true; 
    cancelled= true; 
     pthread_join(timer_thread_id, NULL);  
}

void set_stopping_timer(bool status)
{
    stopping_timer= status; 
}