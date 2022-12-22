#ifndef _MAIN_H_
#define _MAIN_H_

/* Lib Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Header Files */
#include "I2C.h"
#include "SSD1306_OLED.h"
#include "example_app.h"
#include "oled.h"
#include "heartrate.h"
#include "accel.h"
#include "server.h"


void initTracker();
void ALARMhandler(int sig);
void setmainScreenMode(bool val);


#endif