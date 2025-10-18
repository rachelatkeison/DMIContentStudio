#pragma once
#include <JuceHeader.h>
#include "ui/DeviceToolbar.h"
#include "ui/TransportBar.h"
#include "ui/PianoRollComponent.h"
#include "ui/StylePatternEditor.h"
#include "ui/VoiceParameterEditor.h"
#include "model/Project.h"
#include "engine/MidiEngine.h"
#include "util/Commands.h"

class MainComponent : public juce::Component,
                      public juce::ApplicationCommandTarget
{
public:
    MainComponent();
    ~MainComponent() override = default;

    void paint (juce::Graphics& g) override;
    void resized() override;

    juce::ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
    void getAllCommands (juce::Array<juce::CommandID>& c) override;
    void getCommandInfo (juce::CommandID, juce::ApplicationCommandInfo&) override;
    bool perform (const juce::ApplicationCommandTarget::InvocationInfo&) override;

private:
    std::unique_ptr<Project> project;
    MidiEngine engine;

    DeviceToolbar deviceToolbar;
    TransportBar transportBar { engine };

    PianoRollComponent pianoRoll;
    StylePatternEditor styleEditor;
    VoiceParameterEditor voiceEditor { engine };

    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    juce::ApplicationCommandManager commandManager;

    juce::UndoManager undo;

    void newProject();
    void openProject();
    void saveProject (bool saveAs);

    void exportSMF();
    void importSMF();
    void quantizeClip();
};
