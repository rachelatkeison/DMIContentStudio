#pragma once
#include <JuceHeader.h>

class MainComponent;

class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow (const juce::String& name);
    void closeButtonPressed() override;
};
