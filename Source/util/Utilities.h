#pragma once
#include <JuceHeader.h>
namespace Util
{
    inline juce::var toVar (const juce::String& s) { return juce::var (s); }
}
