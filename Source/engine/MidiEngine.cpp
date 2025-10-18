#include <JuceHeader.h>
#include "MidiEngine.h"
#include "Sequencer.h"

MidiEngine::MidiEngine()
{
    sequencer = std::make_unique<Sequencer>();
}

MidiEngine::~MidiEngine()
{
    stop();
}

void MidiEngine::setTempoBPM (double bpm)
{
    if (project) project->tempoBPM = bpm;
}

void MidiEngine::setTimeSignature (int n, int d)
{
    if (project) { project->timeSigNum = n; project->timeSigDen = d; }
}

void MidiEngine::setMidiInput (const juce::String& deviceName)
{
    midiInOwned.reset();

    for (const auto& dev : juce::MidiInput::getAvailableDevices())
    {
        if (dev.name == deviceName)
        {
            midiInOwned = juce::MidiInput::openDevice (dev.identifier, this);
            if (midiInOwned) midiInOwned->start();
            break;
        }
    }
}

void MidiEngine::setMidiOutput (const juce::String& deviceName)
{
    midiOut.reset();

    for (const auto& dev : juce::MidiOutput::getAvailableDevices())
    {
        if (dev.name == deviceName)
        {
            midiOut = juce::MidiOutput::openDevice (dev.identifier);
            break;
        }
    }
}

void MidiEngine::play()
{
    if (! project) return;

    sequencer->prepare (project->songClip, project->tempoBPM, project->timeSigNum, project->timeSigDen);

    startTimer (1);
}

void MidiEngine::stop()
{
    stopTimer();
}

void MidiEngine::toggleRecord()
{
    recording.store (! recording.load());
}

void MidiEngine::auditionStyle()
{
    if (! project) return;

    sequencer->prepareStyle (project->style, project->tempoBPM);
    startTimer (1);
}

void MidiEngine::sendCC (int cc, int value)
{
    if (! project || ! midiOut) return;

    const int ch = juce::jlimit (0, 15, project->voice.channel - 1);
    midiOut->sendMessageNow (juce::MidiMessage::controllerEvent (ch, cc, juce::jlimit (0,127,value)));
}

void MidiEngine::sendProgramChange (int program)
{
    if (! project || ! midiOut) return;

    const int ch = juce::jlimit (0, 15, project->voice.channel - 1);
    midiOut->sendMessageNow (juce::MidiMessage::programChange (ch, juce::jlimit (0,127, program)));
}

void MidiEngine::hiResTimerCallback()
{
    if (! midiOut) return;

    juce::MidiBuffer out;
    sequencer->renderNextBlock (out);

    for (const auto meta : out)
        internalSend (meta.getMessage());
}

void MidiEngine::internalSend (const juce::MidiMessage& m)
{
    if (midiOut)
        midiOut->sendMessageNow (m);
}

void MidiEngine::handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage& m)
{
    if (! recording.load() || ! project) return;

    //clock time to beats using current project tempo
    const double spb     = 60.0 / juce::jmax (1e-6, project->tempoBPM);
    const double beatNow = m.getTimeStamp() / spb;

    //pairfor recording
    if (m.isNoteOn())
    {
        const int vel = juce::jlimit (1, 127, (int) std::round (m.getVelocity() * 127.0f));
        juce::SpinLock::ScopedLockType sl (inLock);
        pendOn.push_back ({ m.getNoteNumber(), beatNow, vel });
    }
    else if (m.isNoteOff())
    {
        const int pitch = m.getNoteNumber();
        juce::SpinLock::ScopedLockType sl (inLock);

        for (int i = (int) pendOn.size() - 1; i >= 0; --i)
        {
            if (pendOn[(size_t)i].pitch == pitch)
            {
                const double startBeat = pendOn[(size_t)i].startBeat;
                const double lenBeats  = juce::jmax (0.125, beatNow - startBeat);
                project->songClip.notes.push_back ({ pitch, pendOn[(size_t)i].vel, startBeat, lenBeats });
                pendOn.erase (pendOn.begin() + i);
                break;
            }
        }
    }
}

void MidiEngine::quantizeClip (MidiClip& clip, double gridBeats, double strength)
{
    const double g = juce::jmax (1e-6, gridBeats);

    for (auto& n : clip.notes)
    {
        //quantize start
        const double qStart = std::round (n.startBeats / g) * g;
        n.startBeats = n.startBeats + (qStart - n.startBeats) * juce::jlimit (0.0, 1.0, strength);

        //quantize end
        const double endBeat = n.startBeats + n.lengthBeats;
        const double qEnd    = std::round (endBeat / g) * g;
        const double newLen  = juce::jmax (0.125, qEnd - n.startBeats);
        n.lengthBeats        = n.lengthBeats + (newLen - n.lengthBeats) * juce::jlimit (0.0, 1.0, strength);
    }
}

