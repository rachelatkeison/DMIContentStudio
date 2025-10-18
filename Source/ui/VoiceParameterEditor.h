#pragma once
#include <JuceHeader.h>
#include "../model/VoiceProgram.h"
#include "../engine/MidiEngine.h"

class VoiceParameterEditor : public juce::Component
{
public:
    explicit VoiceParameterEditor (MidiEngine& engine);
    void resized() override;
    void setVoice (VoiceProgram* v) { voice = v; refreshUI(); }

private:
    MidiEngine& engine;
    VoiceProgram* voice = nullptr;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label>  labels;

    void refreshUI();
    void addParam (const juce::String& name, int cc, float minVal, float maxVal, float init, std::function<void(float)> onChange);
};
