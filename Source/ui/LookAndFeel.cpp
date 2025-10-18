#include <JuceHeader.h>
#include "LookAndFeel.h"

DmiLookAndFeel::DmiLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, juce::Colour (0x111213));
    setColour (juce::DocumentWindow::textColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, juce::Colour (0x292c2f));
    setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    setColour (juce::TextButton::textColourOffId, juce::Colours::white);
    setColour (juce::Slider::thumbColourId, juce::Colour (0xffd6ff00));
    setColour (juce::Slider::trackColourId, juce::Colours::grey);
    setColour (juce::TabbedComponent::backgroundColourId, juce::Colour (0x141618));
}
