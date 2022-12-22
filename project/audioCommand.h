#ifndef AUDIO_HEADER
#define AUDIO_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char * readTranscription();

int whichWordSaid(char * transcribed);
void controlTrackerMode(int modeToSet);


#endif 