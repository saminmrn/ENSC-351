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
#include "timer.h"


void setScreen();

void displayHeartRate(int heartRate);

void displaySteps(int steps);

void displayVoiceCommand(int steps);

void displayStats(int caloriesBurnt, int distance);

void displayCalories(int caloriesBurnt);

void displayMainScreen();

void displayVoiceControlError();

void displayVoiceControl();

void displayWorkoutComplete();

void displayTimerCountdown();

void displayTimerSetSec();

void displayTimerSetMin();

void displayWorkoutCancelled();
