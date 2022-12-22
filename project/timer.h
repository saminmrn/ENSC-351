/* timer.h */
#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdbool.h>
void start_timer();
int currentTime();
void cleanup_timer(); 
void cancel_timer(); 
void set_timer_seconds(int new_timer); 
void set_timer_minutes(int new_timer); 
int get_timer_seconds(); 
int get_timer_minutes(); 
void set_stopping_timer(bool status); 

#endif