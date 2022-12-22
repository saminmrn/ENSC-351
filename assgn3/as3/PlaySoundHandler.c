#include "PlaySoundHandler.h"
#include "audioMixer.h"

// increase by 5
void increaseVolume()
{
    int previous_volume = AudioMixer_getVolume();
    previous_volume += 5;
    if (previous_volume >= 100)
    {
        previous_volume = 100;
    }
    AudioMixer_setVolume(previous_volume);
}
// decrease by 5
void decreaseVolume()
{
    int previous_volume = AudioMixer_getVolume();
    previous_volume -= 5;
    if (previous_volume <= 0)
    {
        previous_volume = 0;
    }
    AudioMixer_setVolume(previous_volume);
}
// get new volume
int getNewVolume()
{
    int newVolume = AudioMixer_getVolume();
    if (newVolume >= 100)
    {
        newVolume = 99;
    }
    return newVolume;
}
// increase by 5
void increaseTempo()
{
    int previous_tempo = AudioMixer_get_BPM();
    AudioMixer_set_BPM(previous_tempo + 5);
}
// decrease by 5
void decreaseTempo()
{
    int previous_tempo = AudioMixer_get_BPM();
    AudioMixer_set_BPM(previous_tempo - 5);
}
// get new tempo
int getNewTempo()
{
    int newTempo = AudioMixer_get_BPM();
    return newTempo;
}
