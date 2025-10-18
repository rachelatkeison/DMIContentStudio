#include <JuceHeader.h>
#include "App.h"
#include "MainWindow.h"

void DMIApplication::initialise (const juce::String&)
{
    mainWindow = std::make_unique<MainWindow> (getApplicationName());
    mainWindow->setVisible (true);
}

void DMIApplication::shutdown() { mainWindow = nullptr; }
void DMIApplication::systemRequestedQuit() { quit(); }

START_JUCE_APPLICATION (DMIApplication)
