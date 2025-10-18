#pragma once
#include <JuceHeader.h>

class DeviceToolbar : public juce::Component
{
public:
    std::function<void(const juce::String&)> onMidiInputSelected;
    std::function<void(const juce::String&)> onMidiOutputSelected;
    std::function<void(double)> onTempoChanged;
    std::function<void(int,int)> onTimeSigChanged;

    DeviceToolbar();
    void resized() override;

private:
    juce::ComboBox midiIn, midiOut;
    juce::Label tempoLabel {"tempo","Tempo"};
    juce::Slider tempoSlider;
    juce::Label tsLabel {"ts","TS"};
    juce::ComboBox timeSig;

    void refreshDevices();
};
