// functions to start and clean up the beat box thread
#ifndef BEAT_BOX_H
#define BEAT_BOX_H

void BeatBox_init();
void BeatBox_cleanup();

// function used to only play the drum, snare and hihat once
// it is called in buttons.c
void set_count(int c);
void oneBeat(int i);

void setStatsBB();
int getSamplesBB();
double getAverageBB();
double getMinBB();
double getMaxBB();

#endif