#pragma once
#include <JuceHeader.h>
#include "../model/MidiClip.h"
#include "../model/StylePattern.h"

class Sequencer
{
public:
    enum class Mode { None, Song, Style };

    Sequencer() = default;
    ~Sequencer() = default;

    void prepare (const MidiClip& clip, double bpm, int tsNum, int tsDen);
    void prepareStyle (const StylePattern& pattern, double bpm);

    //fills MidiBuffer with note events for this time slice
    void renderNextBlock (juce::MidiBuffer& out);

    Mode getMode() const noexcept { return mode; }

private:
    //state
    Mode mode { Mode::None };
    double bpm { 120.0 };

    //timing
    double prevBeat { 0.0 };
    double currBeat { 0.0 };
    double lastSecStamp { 0.0 };
    double styleAccumSec { 0.0 };

    // style pa ttern position
    int styleStep { 0 };

    //working copies of clip
    MidiClip clipCopy;
    StylePattern styleCopy;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sequencer)
};

