#include <JuceHeader.h>
#include "TransportBar.h"
#include "../engine/MidiEngine.h"

TransportBar::TransportBar (MidiEngine&)
{
    addAndMakeVisible (play);
    addAndMakeVisible (stop);
    addAndMakeVisible (record);

    play.onClick   = [this]{ if (onPlay) onPlay(); };
    stop.onClick   = [this]{ if (onStop) onStop(); };
    record.onClick = [this]{ if (onRecord) onRecord(); };
}
void TransportBar::resized()
{
    auto r = getLocalBounds().reduced (6);
    play.setBounds (r.removeFromLeft (70));
    r.removeFromLeft (6);
    stop.setBounds (r.removeFromLeft (70));
    r.removeFromLeft (6);
    record.setBounds (r.removeFromLeft (70));
}
