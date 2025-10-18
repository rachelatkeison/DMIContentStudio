#pragma once
#include <JuceHeader.h>

class DMIApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return "DMI Content Studio"; }
    const juce::String getApplicationVersion() override { return "0.2.0"; }
    bool moreThanOneInstanceAllowed() override          { return true; }

    void initialise (const juce::String&) override;
    void shutdown() override;
    void systemRequestedQuit() override;

private:
    std::unique_ptr<juce::DocumentWindow> mainWindow;
};
