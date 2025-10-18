#pragma once
#include <JuceHeader.h>
#include <vector>

struct MidiNote
{
    int pitch = 60;
    int velocity = 100;
    double startBeats = 0.0;
    double lengthBeats = 1.0;
};

struct MidiClip
{
    std::vector<MidiNote> notes;
    double lengthBeats = 16.0;
};
