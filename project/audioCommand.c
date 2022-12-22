

#include "audioCommand.h"
#include "oled.h"
#include "accel.h"
#include "heartrate.h"
#include "main_timer.h"
#include "main.h"


// list of modes/ functionalities the tracker can display 
char * trackerModes[] = {
    "Timer.\0",   // (0) start the timer thread
    "Stop.\0",    // (1) stop the timer prematurely 
    "Steps.\0",   // (2) display step counter, step thread runs always 
    "Heart.\0",   // (3) display heart rate bpm, thread runs always
    "Stat.\0",    // (4) display stats from the webserver based on heart rate and steps
    "Shut down.\0"// (5) stop threads 
    // (-1) will be any other mode detected from the transcription 
};


char * readTranscription()
{
    FILE* ptr;
    ptr = fopen("transcription.txt", "r");
 
    if (NULL == ptr) {
        printf("file can't be opened \n");
    }
 
 
    char * transcWord;
    int index = 0;
    transcWord = (char*)malloc(1*sizeof(char));

    while (!feof(ptr)) {
        transcWord[index] = fgetc(ptr);
        index++;
    }
    
    //append null character at the end of the string
    transcWord[index-1] = '\0'; 

    fclose(ptr);
    return transcWord;
}

int whichWordSaid(char * transcribed)
{

    int numModes = sizeof(trackerModes)/sizeof(trackerModes[0]);
    for(int i = 0; i <numModes; i++){
        int checkMode = strcmp(transcribed, trackerModes[i]);

        // a match has been found 
        if(checkMode == 0) 
        {
            printf("You chose the '%s' mode of the tracker \n", trackerModes[i]);
            return i;
        }
    }
    return -1;
}

// joy stick has been moved to the right, start the recording and transcription 
void controlTrackerMode(int modeToSet)
{
    int mode = modeToSet;
    printf("mode: %d", mode);


    // //handle error
    // if(transcribed == NULL || mode == -1)
    // {  
    //     displayVoiceControlError();
    //     sleep(5);
    //     setmainScreenMode(true);
        
    // }

    // Mode (0) start the timer thread
    if(mode == 0)
    {
        displayTimerSetMin();
        setmainScreenMode(true);
        //do the timer shit 
    }

    // Mode(1) stop the timer prematurely 
    if(mode == 1)
    {
        main_timer_shutdown();
    }

    // Mode (2) display step counter, step thread runs always 
    if(mode == 2)
    {
        displaySteps(getSteps());
        setmainScreenMode(true);
    }

    // Mode (3) display heart rate bpm, thread runs always
    if(mode == 3)
    {
        displayHeartRate(getHeartRate());
         setmainScreenMode(true);
    }

    // Mode (4) display stats from the webserver based on heart rate and steps
    if(mode == 4)
    {
        //display stats
        displayStats(getCalories(), getDistanceInKm());
        setmainScreenMode(true);
    }

    // Mode (5) stop all threads 
    if(mode == 5)
    {
        setmainScreenMode(true);
        // free(transcribed);
    }

    
}

