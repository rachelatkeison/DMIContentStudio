#pragma once
#include <JuceHeader.h>
#include "MidiClip.h"
#include "StylePattern.h"
#include "VoiceProgram.h"

struct Project
{
    juce::String name = "Untitled";
    juce::String file;
    double tempoBPM = 120.0;
    int timeSigNum = 4, timeSigDen = 4;

    MidiClip     songClip;
    StylePattern style;
    VoiceProgram voice;
};
