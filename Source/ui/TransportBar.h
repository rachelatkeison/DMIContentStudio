#pragma once
#include <JuceHeader.h>
class MidiEngine;

class TransportBar : public juce::Component
{
public:
    explicit TransportBar (MidiEngine& engine);
    void resized() override;

    std::function<void()> onPlay, onStop, onRecord;

private:
    juce::TextButton play {"Play"}, stop {"Stop"}, record {"Rec"};
};
