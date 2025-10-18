#pragma once
#include <JuceHeader.h>

struct VoiceProgram
{
    int attack=10, decay=40, sustain=100, release=30;
    int cutoff=90, res=40, vibrato=0, velCurve=64;

    int programNumber = 0;
    int channel = 1;   
};
