#include <JuceHeader.h>
#include "PianoRollComponent.h"

PianoRollComponent::PianoRollComponent()
{
    setOpaque (true);
}
void PianoRollComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0x15171a));
    if (!clip) return;

    auto r = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0x222529));
    for (int x = 0; x < r.getWidth(); x += (int)pxPerBeat) g.drawVerticalLine (x, 0.0f, r.getHeight());
    for (int y = 0; y < r.getHeight(); y += (int)pxPerSemitone) g.drawHorizontalLine (y, 0.0f, r.getWidth());

    g.setColour (juce::Colour (0xff62d1ff));
    for (int i=0; clip && i<(int)clip->notes.size(); ++i)
    {
        auto& n = clip->notes[(size_t)i];
        auto x = (float)(n.startBeats * pxPerBeat);
        auto w = (float)(n.lengthBeats * pxPerBeat);
        auto y = (float)((keyMax - n.pitch) * pxPerSemitone);
        juce::Rectangle<float> rr { x, y, juce::jmax (w, 6.0f), (float)pxPerSemitone-1.0f };
        g.fillRoundedRectangle (rr, 2.0f);
    }
}
void PianoRollComponent::resized() {}

int PianoRollComponent::yToPitch (int y) const
{
    int p = keyMax - int ((double)y / pxPerSemitone);
    return juce::jlimit (keyMin, keyMax, p);
}
double PianoRollComponent::xToBeat (int x) const { return (double)x / pxPerBeat; }

int PianoRollComponent::hitTestNote (int x, int y) const
{
    if (!clip) return -1;
    for (int i=(int)clip->notes.size()-1; i>=0; --i)
    {
        auto& n = clip->notes[(size_t)i];
        float rx = (float)(n.startBeats * pxPerBeat);
        float rw = (float)(n.lengthBeats * pxPerBeat);
        float ry = (float)((keyMax - n.pitch) * pxPerSemitone);
        juce::Rectangle<float> rr { rx, ry, juce::jmax (rw, 6.0f), (float)pxPerSemitone-1.0f };
        if (rr.contains ((float)x, (float)y)) return i;
    }
    return -1;
}

void PianoRollComponent::mouseDown (const juce::MouseEvent& e)
{
    if (!clip) return;

    if (e.mods.isRightButtonDown())
    {
        int idx = hitTestNote (e.x, e.y);
        if (idx >= 0 && onDeleteNote) onDeleteNote (idx);
        return;
    }

    int pitch = yToPitch (e.y);
    double start = xToBeat (e.x);
    MidiNote nn { pitch, 100, start, 1.0 };

    if (onAddNote) onAddNote (nn);
    lastIndex = (int)clip->notes.size() - 1;
    repaint();
}
void PianoRollComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (!clip || lastIndex < 0 || lastIndex >= (int)clip->notes.size()) return;
    auto& n = clip->notes[(size_t)lastIndex];
    double newLen = juce::jmax (0.125, xToBeat (e.x) - n.startBeats);
    if (onResizeNote) onResizeNote (lastIndex, newLen);
    repaint();
}
