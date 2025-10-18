#pragma once
#include <JuceHeader.h>
#include "../model/MidiClip.h"

namespace SMF
{
    bool exportClipToSMF (const MidiClip& clip, const juce::File& file, int tpq=480);
    std::unique_ptr<MidiClip> importClipFromSMF (const juce::File& file, int tpqOut=480);
}
