#include <JuceHeader.h>
#include "VoiceParameterEditor.h"

VoiceParameterEditor::VoiceParameterEditor (MidiEngine& eng) : engine (eng)
{
    setOpaque (false);
}
void VoiceParameterEditor::addParam (const juce::String& name, int cc, float minV, float maxV, float init, std::function<void(float)> onChange)
{
    auto* s = new juce::Slider();
    s->setRange (minV, maxV, 1.0);
    s->setValue (init, juce::dontSendNotification);
    s->onValueChange = [this, s, cc, onChange]
    {
        float v = (float) s->getValue();
        if (onChange) onChange (v);
        engine.sendCC (cc, (int) juce::jlimit (0,127,(int)v));
    };
    sliders.add (s); addAndMakeVisible (s);

    auto* l = new juce::Label ({}, name);
    l->setJustificationType (juce::Justification::centred);
    labels.add (l); addAndMakeVisible (l);
}
void VoiceParameterEditor::refreshUI()
{
    for (auto* s : sliders) s->setVisible (false);
    for (auto* l : labels)  l->setVisible (false);
    sliders.clear (true); labels.clear (true);
    if (!voice) return;

    addParam ("Attack", 73, 0,127, voice->attack,  [this](float v){ if (voice) voice->attack  = (int)v; });
    addParam ("Decay",  75, 0,127, voice->decay,   [this](float v){ if (voice) voice->decay   = (int)v; });
    addParam ("Sustain",70, 0,127, voice->sustain, [this](float v){ if (voice) voice->sustain = (int)v; });
    addParam ("Release",72, 0,127, voice->release, [this](float v){ if (voice) voice->release = (int)v; });
    addParam ("Cutoff", 74, 0,127, voice->cutoff,  [this](float v){ if (voice) voice->cutoff  = (int)v; });
    addParam ("Res",    71, 0,127, voice->res,     [this](float v){ if (voice) voice->res     = (int)v; });
    addParam ("Vibrato",1 , 0,127, voice->vibrato, [this](float v){ if (voice) voice->vibrato = (int)v; });
    addParam ("VelCurve",7, 0,127, voice->velCurve,[this](float v){ if (voice) voice->velCurve= (int)v; });
    resized();
}
void VoiceParameterEditor::resized()
{
    auto r = getLocalBounds().reduced (12);
    int cols = 4;
    int rows = (int) std::ceil (sliders.size() / (double) cols);
    int cellW = r.getWidth() / cols;
    int cellH = r.getHeight() / juce::jmax (1, rows);

    for (int i=0;i<sliders.size();++i)
    {
        int cx = i % cols, cy = i / cols;
        auto cell = juce::Rectangle<int> (r.getX()+cx*cellW, r.getY()+cy*cellH, cellW, cellH);
        labels[i]->setBounds (cell.removeFromTop (22));
        sliders[i]->setBounds (cell.reduced (10));
    }
}
