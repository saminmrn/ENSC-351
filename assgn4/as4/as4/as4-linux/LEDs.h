// LEDs.h has the numbers and letter displays for the LED matrix
#ifndef _LEDS_H_
#define _LEDS_H_
#include "sharedDataStruct.h"

void disp_init();
void Display_waveform(sharedMemStruct_t shared_Data, int data_size); 
void clearLEDs(); 
#endif