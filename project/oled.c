/*
 * MIT License

Copyright (c) 2017 DeeplyEmbedded

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 * example_app.c
 *
 *  Created on  : Sep 6, 2017
 *  Author      : Vinay Divakar
 *  Website     : www.deeplyembedded.org
 */

/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "SSD1306_OLED.h"
#include "I2C.h"
#include <time.h>

//#include "HR.h"
#include "accel.h"
#include "oled.h"
// volatile unsigned char flag = 0;

bool display;

/* MACRO's */
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

static pthread_t oledThread_id;

/* Extern volatile */
extern volatile unsigned char flag;

/* Bit Map - Taken from Adafruit SSD1306 OLED Library  */
static const unsigned char logo16_glcd_bmp[] = {
    0b00000000, 0b11000000,
    0b00000001, 0b11000000,
    0b00000001, 0b11000000,
    0b00000011, 0b11100000,
    0b11110011, 0b11100000,
    0b11111110, 0b11111000,
    0b01111110, 0b11111111,
    0b00110011, 0b10011111,
    0b00011111, 0b11111100,
    0b00001101, 0b01110000,
    0b00011011, 0b10100000,
    0b00111111, 0b11100000,
    0b00111111, 0b11110000,
    0b01111100, 0b11110000,
    0b01110000, 0b01110000,
    0b00000000, 0b00110000
};

void setScreen() {
    clearDisplay();
    setTextSize(1);
    setTextColor(WHITE);
    setCursor(1,0);
}

// *************** MAIN MODES ******************//
void displayMainScreen()
{
    setScreen();
    print_strln("your fitness tracker!");
    print_strln(" ");
    print_strln("RIGHT->"); 
    print_strln("start voice control");
    Display();
}

void displayHeartRate(int heartRate) {
    setScreen();

    //initialize the string for displaying heartrate
    int heartBeat = heartRate;
    char *buf;
    size_t sz;      
    sz = snprintf(NULL, 0, "current heartrate %d", heartBeat);
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "current heartrate %d", heartBeat);

    print_strln("You chose: Heart Rate");
    print_strln("");
    print_strln("          <3  ");
    print_strln("");
    print_strln("");
    print_strln("");
    print_strln(buf);
    Display();

};

void displayVoiceControl()
{
    setScreen();
    print_strln(" SPEAK INTO THE MIC");
    print_strln("");
    print_strln("         ...  ");
    print_strln("      Please Wait ");
    print_strln("");
    print_strln("");
    Display();
}

void displayVoiceControlError()
{
    setScreen();
    print_strln("sorry!");
    print_strln("we couldn't hear you");
    print_strln("");
    print_strln("");
    print_strln("   please try again!");
    print_strln("          :(  ");
    print_strln("");
    print_strln("");
    Display();
    
}

void displaySteps(int steps) {
    setScreen();

    //initialize the string for displaying steps
    char *buf;
    size_t sz;
    printf("%d", steps);
    sz = snprintf(NULL, 0, "total steps %d", steps);
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "total steps %d", steps);

    //print to screen
    print_strln("You chose: Steps!");
    print_strln("");
    print_strln("");
    print_strln("           O   ");
    print_strln("          -I-  ");
    print_strln("          / \\  ");
    print_strln("");
    print_strln(buf);
    Display();
    // sleep(5);
};


void displayStats(int caloriesBurnt, int distance) {
    setScreen();
    size_t sz;      
    sz = snprintf(NULL, 0, "Calories burnt %d", caloriesBurnt);
    char *buf;
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "Calories burnt %d", caloriesBurnt);

    size_t sz2;  
    sz2 = snprintf(NULL, 0, "distance walked %d", distance);
    char *buf2;
    buf2 = (char *)malloc(sz2 + 1); /* make sure you check for != NULL in real code */
    snprintf(buf2, sz2+1, "distance walked %d", distance);
    print_strln("You chose: Stats! ");
    print_strln("");
    print_strln(buf);
    print_strln("");
    print_strln(buf2);

    Display();
}

// *************** TIMER MODES ******************//

void displayTimerSetMin()
{
    
    size_t sz;      
    sz = snprintf(NULL, 0, "timer set to %d:%d",get_timer_minutes(), get_timer_seconds());
    char *buf;
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "timer set to %d:%d", get_timer_minutes(), get_timer_seconds());
    setScreen();
    print_strln(buf);
    print_strln("");
    print_strln("UP/DOWN:");
    print_strln("-inc/dec time in MIN");
    print_strln("");
    print_strln("set with grey button");
    Display();
}

void displayTimerSetSec()
{
    size_t sz;      
    sz = snprintf(NULL, 0, "timer set to %d:%d",get_timer_minutes(), get_timer_seconds());
    char *buf;
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "timer set to %d:%d", get_timer_minutes(), get_timer_seconds());
    setScreen();
    print_strln(buf);
    print_strln("");
    print_strln("UP/DOWN:");
    print_strln("-inc/dec time in SEC");
    print_strln("");
    print_strln("LEFT to start!");
    Display();
}

void displayTimerSetTo() {
    setScreen();
    size_t sz;      
    sz = snprintf(NULL, 0, "timer set to %d:%d",get_timer_minutes(), get_timer_seconds());
    char *buf;
    buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
    snprintf(buf, sz+1, "timer set to %d:%d", get_timer_minutes(), get_timer_seconds());
    print_strln(buf);
    print_strln("");
    print_strln("UP/DOWN:");
    print_strln("-inc/dec time in SEC");
    print_strln("");
    print_strln("LEFT to start!");
    Display();
}

void displayTimerCountdown()
{
    setScreen();
    print_strln("  TIMER COUNT DOWN");
    print_strln("");
    print_strln("         ...  ");
    print_strln("");
    print_strln("    YOU GOT THIS! ");
    print_strln("");
    print_strln("");
    Display();
}

void displayWorkoutComplete()
{
    setScreen();
    print_strln("   WORKOUT COMPLETE");
    print_strln("");
    print_strln("");
    print_strln("         !!!  ");
    print_strln("");
    print_strln("      Great Job :) ");
    print_strln("");
    Display();

}

void displayWorkoutCancelled()
{
    setScreen();
    print_strln("WORKOUT TIMER CANCELLED");
    print_strln("");
    print_strln("");
    print_strln("         !!!  ");
    print_strln("");
    print_strln("      maybe next time ");
    print_strln("");
    Display();

}


static void *OLEDThread(void *_) {
    // assume the device has already been initilialized

    while(display == true) {
        // displaySteps();
        displayHeartRate(12);
    }
}





