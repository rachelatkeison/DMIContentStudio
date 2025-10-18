#pragma once
#include <JuceHeader.h>
#include "../model/Project.h"

class Sequencer;

class MidiEngine : private juce::HighResolutionTimer,
                   private juce::MidiInputCallback
{
public:
    MidiEngine();
    ~MidiEngine() override;

    void setProject (Project* p) { project = p; }
    void setTempoBPM (double bpm);
    void setTimeSignature (int num, int den);

    void setMidiInput (const juce::String& deviceName);
    void setMidiOutput (const juce::String& deviceName);

    void play();
    void stop();
    void toggleRecord();

    void auditionStyle();

    void sendCC (int cc, int value);
    void sendProgramChange (int program);

    void quantizeClip (MidiClip& clip, double gridBeats, double strength = 1.0);

private:
    Project* project = nullptr;

    std::unique_ptr<Sequencer> sequencer;
    std::unique_ptr<juce::MidiInput>  midiInOwned;
    std::unique_ptr<juce::MidiOutput> midiOut;

    std::atomic<bool> recording { false };

    struct PendingOn { int pitch; double startBeat; int vel; };
    juce::SpinLock inLock;
    std::vector<PendingOn> pendOn;

    void hiResTimerCallback() override;
    void internalSend (const juce::MidiMessage& m);

    void handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage&) override;
    void handlePartialSysexMessage (juce::MidiInput*, const juce::uint8*, int, double) override {}
};
