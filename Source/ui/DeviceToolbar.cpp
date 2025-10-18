#include <JuceHeader.h>
#include "DeviceToolbar.h"

DeviceToolbar::DeviceToolbar()
{
    addAndMakeVisible (midiIn);
    addAndMakeVisible (midiOut);
    addAndMakeVisible (tempoLabel);
    addAndMakeVisible (tempoSlider);
    addAndMakeVisible (tsLabel);
    addAndMakeVisible (timeSig);

    tempoSlider.setRange (40.0, 240.0, 0.1);
    tempoSlider.setValue (120.0);
    tempoSlider.onValueChange = [this]{ if (onTempoChanged) onTempoChanged (tempoSlider.getValue()); };

    timeSig.addItem ("4/4", 1);
    timeSig.addItem ("3/4", 2);
    timeSig.addItem ("6/8", 3);
    timeSig.onChange = [this]
    {
        auto text = timeSig.getText();
        int n=4,d=4;
        if      (text == "3/4") { n=3; d=4; }
        else if (text == "6/8") { n=6; d=8; }
        if (onTimeSigChanged) onTimeSigChanged (n,d);
    };
    timeSig.setSelectedId (1);

    refreshDevices();
    midiIn.onChange  = [this]{ if (onMidiInputSelected)  onMidiInputSelected (midiIn.getText()); };
    midiOut.onChange = [this]{ if (onMidiOutputSelected) onMidiOutputSelected (midiOut.getText()); };
}
void DeviceToolbar::refreshDevices()
{
    midiIn.clear(); midiOut.clear();
    auto inputs = juce::MidiInput::getAvailableDevices();
    for (int i=0;i<inputs.size();++i) midiIn.addItem (inputs[i].name, i+1);
    auto outputs = juce::MidiOutput::getAvailableDevices();
    for (int i=0;i<outputs.size();++i) midiOut.addItem (outputs[i].name, i+1);
    if (midiIn.getNumItems()>0)  midiIn.setSelectedId (1, juce::dontSendNotification);
    if (midiOut.getNumItems()>0) midiOut.setSelectedId (1, juce::dontSendNotification);
}
void DeviceToolbar::resized()
{
    auto r = getLocalBounds().reduced (8);
    midiIn.setBounds (r.removeFromLeft (220));
    r.removeFromLeft (8);
    midiOut.setBounds (r.removeFromLeft (220));
    r.removeFromLeft (8);
    tempoLabel.setBounds (r.removeFromLeft (50));
    tempoSlider.setBounds (r.removeFromLeft (160));
    r.removeFromLeft (8);
    tsLabel.setBounds (r.removeFromLeft (30));
    timeSig.setBounds (r.removeFromLeft (80));
}
