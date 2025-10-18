#pragma once
#include <JuceHeader.h>

namespace NRPN
{
    inline void sendNRPN (juce::MidiOutput* out, int channel0, int nrpnMSB, int nrpnLSB, int value14)
    {
        if (!out) return;
        auto cc = [&](int num, int val){ out->sendMessageNow (juce::MidiMessage::controllerEvent (channel0, num, juce::jlimit (0,127,val))); };
        cc(99, nrpnMSB); cc(98, nrpnLSB);
        cc(6,  (value14 >> 7) & 0x7F);
        cc(38, value14 & 0x7F);
        cc(99, 127); cc(98, 127);
    }
}
