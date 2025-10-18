#pragma once
#include <JuceHeader.h>
#include "../model/MidiClip.h"

class PianoRollComponent : public juce::Component
{
public:
    PianoRollComponent();

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setClip (MidiClip* c) { clip = c; repaint(); }

    std::function<void(const MidiNote&)> onAddNote;
    std::function<void(int index, double newLength)> onResizeNote;
    std::function<void(int index)> onDeleteNote;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;

private:
    MidiClip* clip = nullptr;
    double pxPerBeat = 120.0;
    double pxPerSemitone = 12.0;
    int keyMin = 36, keyMax = 96;
    int lastIndex = -1;

    int yToPitch (int y) const;
    double xToBeat (int x) const;
    int hitTestNote (int x, int y) const;
};
