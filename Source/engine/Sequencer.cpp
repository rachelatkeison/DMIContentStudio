#include <JuceHeader.h>
#include "Sequencer.h"

namespace
{
    inline double secPerBeat (double bpm) { return 60.0 / juce::jmax (1e-6, bpm); }
}

void Sequencer::prepare (const MidiClip& clip, double b, int /*tsNum*/, int /*tsDen*/)
{
    mode            = Mode::Song;
    clipCopy        = clip;
    styleCopy       = {};
    bpm             = b;
    prevBeat        = 0.0;
    currBeat        = 0.0;
    lastSecStamp    = juce::Time::getMillisecondCounterHiRes() * 0.001;
    styleStep       = 0;
    styleAccumSec   = 0.0;
}

void Sequencer::prepareStyle (const StylePattern& pattern, double b)
{
    mode            = Mode::Style;
    clipCopy        = {};
    styleCopy       = pattern;
    bpm             = b;
    prevBeat        = 0.0;
    currBeat        = 0.0;
    lastSecStamp    = juce::Time::getMillisecondCounterHiRes() * 0.001;
    styleStep       = 0;
    styleAccumSec   = 0.0;
}

void Sequencer::renderNextBlock (juce::MidiBuffer& out)
{
    out.clear();

    const double nowSec   = juce::Time::getMillisecondCounterHiRes() * 0.001;
    const double dt       = juce::jmax (0.0, nowSec - lastSecStamp);
    lastSecStamp          = nowSec;

    if (mode == Mode::None)
        return;

    const double beatsDelta = dt / secPerBeat (bpm);
    prevBeat = currBeat;
    currBeat += beatsDelta;

    if (mode == Mode::Song)
    {
        //search all notes every tick n fine for small medium clips.
        for (const auto& n : clipCopy.notes)
        {
            const double onBeat  = n.startBeats;
            const double offBeat = n.startBeats + n.lengthBeats;

            const bool crossedOn  = (onBeat  >= prevBeat && onBeat  < currBeat);
            const bool crossedOff = (offBeat >= prevBeat && offBeat < currBeat);

            if (crossedOn)
                out.addEvent (juce::MidiMessage::noteOn  (/*chan*/ 0, n.pitch, (juce::uint8) juce::jlimit (1,127,n.velocity)), 0);

            if (crossedOff)
                out.addEvent (juce::MidiMessage::noteOff (/*chan*/ 0, n.pitch), 0);
        }

        const double clipLen = juce::jmax (1.0, clipCopy.lengthBeats);
        if (currBeat >= clipLen)
        {
            prevBeat = std::fmod (prevBeat, clipLen);
            currBeat = std::fmod (currBeat, clipLen);
        }
    }
    else if (mode == Mode::Style)
    {
        //16th note grid stepping
        styleAccumSec += dt;
        const double stepSec = secPerBeat (bpm) * (1.0 / 4.0);

        while (styleAccumSec >= stepSec)
        {
            for (int r = 0; r < StylePattern::rows; ++r)
            {
                if (styleCopy.grid[r][styleStep])
                {
                    const int pitch = styleCopy.pitchForCell (r, styleStep);
                    const int ch    = juce::jlimit (0, 15, styleCopy.channelForRow (r) - 1);

                    out.addEvent (juce::MidiMessage::noteOn  (ch, pitch, (juce::uint8)100), 0);
                    //add a short note-off
                    out.addEvent (juce::MidiMessage::noteOff (ch, pitch), 120);
                }
            }

            styleStep     = (styleStep + 1) % StylePattern::cols;
            styleAccumSec -= stepSec;
        }
    }
}

