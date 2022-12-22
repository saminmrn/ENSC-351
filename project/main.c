/*
 * Main.c
 *
 *  Created on  : Sep 6, 2017
 *  Author      : Vinay Divakar
 *  Description : Example usage of the SSD1306 Driver API's
 *  Website     : www.deeplyembedded.org
 */


/* Oh Compiler-Please leave me as is */
volatile unsigned char flag = 0;
#include "main.h"
#include "audioCommand.h"
#include "joystick.h"
#include "buttons.h"
#include "main_timer.h"
#include "server.h"
#include "accel.h"

bool mainScreenMode = true;

void setmainScreenMode(bool val)
{
    mainScreenMode = val;
}


int main()
{
    initTracker();
}


/* Alarm Signal Handler */
void ALARMhandler(int sig)
{
    /* Set flag */
    flag = 5;
}


// initialize the tracker, display the main screen 
void initTracker()
{

    //initialize threads that will be ongoing 

    
    // start_heartrate();
    accel_init();
    // printf("%d", getSteps());
    // sleep(5);
    // printf("%d", getMagnitude());
    main_timer();
    server_thread_init();

    /* Initialize I2C bus and connect to the I2C Device */
    if(init_i2c_dev(I2C_DEV2_PATH, SSD1306_OLED_ADDR) == 0)
    {
        printf("(Main)i2c-2: Bus Connected to SSD1306\r\n");
    }
    else
    {
        printf("(Main)i2c-2: OOPS! Something Went Wrong\r\n");
        exit(1);
    }

     // /* Register the Alarm Handler */
    signal(SIGALRM, ALARMhandler);

    display_Init_seq();
    displayMainScreen();

    sleep(5);
 
    while(true) {

    }

    
    

     /* Run SDD1306 Initialization Sequence */

    // Display the main screen until trigger 

    

    
    // controlTrackerMode();

    // while(mainScreenMode)
    // { 
    //     displayMainScreen();
        
    // }

}

