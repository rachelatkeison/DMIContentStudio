#include <JuceHeader.h>
#include "MainWindow.h"
#include "MainComponent.h"

MainWindow::MainWindow (const juce::String& name)
    : juce::DocumentWindow (
        name,
        juce::Desktop::getInstance().getDefaultLookAndFeel()
            .findColour (juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setResizable (true, true);
    setContentOwned (new MainComponent(), true);
    centreWithSize (getWidth(), getHeight());
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
