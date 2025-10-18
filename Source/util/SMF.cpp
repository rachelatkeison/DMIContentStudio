#include <JuceHeader.h>
#include "SMF.h"

namespace SMF
{
    bool exportClipToSMF (const MidiClip& clip, const juce::File& file, int tpq)
    {
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote (tpq);

        juce::MidiMessageSequence seq;
        for (auto& n : clip.notes)
        {
            double tickStart = n.startBeats * tpq;
            double tickEnd   = (n.startBeats + n.lengthBeats) * tpq;

            seq.addEvent (juce::MidiMessage::noteOn (1, n.pitch, (juce::uint8)n.velocity), tickStart);
            seq.addEvent (juce::MidiMessage::noteOff (1, n.pitch), tickEnd);
        }

        seq.updateMatchedPairs();
        seq.sort();

        midiFile.addTrack (seq);
        juce::FileOutputStream outStream (file);
        if (! outStream.openedOk()) return false;

        return midiFile.writeTo (outStream);
    }

    std::unique_ptr<MidiClip> importClipFromSMF (const juce::File& file, int /*tpqOut*/)
    {
        juce::FileInputStream inStream (file);
        if (! inStream.openedOk()) return nullptr;

        juce::MidiFile midiFile;
        if (! midiFile.readFrom (inStream)) return nullptr;

        midiFile.convertTimestampTicksToSeconds();

        auto clip = std::make_unique<MidiClip>();

        if (midiFile.getNumTracks() > 0)
        {
            const juce::MidiMessageSequence* seq = midiFile.getTrack (0);
            if (seq != nullptr)
            {
                double secPerBeat = 60.0 / clip->lengthBeats;
                for (int i=0; i < seq->getNumEvents(); ++i)
                {
                    auto& holder = seq->getEventPointer(i)->message;
                    if (holder.isNoteOn())
                    {
                        double beat = holder.getTimeStamp() / secPerBeat;
                        int pitch  = holder.getNoteNumber();
                        int vel    = (int)(holder.getVelocity() * 127.f);
                        
                        clip->notes.push_back ({ pitch, vel, beat, 1.0 });
                    }
                }
            }
        }

        return clip;
    }
}

