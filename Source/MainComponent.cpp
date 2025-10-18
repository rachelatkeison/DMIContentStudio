#include <JuceHeader.h>
#include "MainComponent.h"
#include "ui/LookAndFeel.h"
#include "util/Persist.h"
#include "util/Undoable.h"
#include "util/SMF.h"

namespace { constexpr int kTopBarHeight = 44; }

MainComponent::MainComponent()
{
    setSize (1200, 750);

    project = std::make_unique<Project>();
    engine.setProject (project.get());

    addAndMakeVisible (deviceToolbar);
    addAndMakeVisible (transportBar);

    tabs.addTab ("Song",  juce::Colours::transparentBlack, &pianoRoll,  false);
    tabs.addTab ("Style", juce::Colours::transparentBlack, &styleEditor, false);
    tabs.addTab ("Voice", juce::Colours::transparentBlack, &voiceEditor, false);
    addAndMakeVisible (tabs);

    commandManager.registerAllCommandsForTarget (this);
    addKeyListener (commandManager.getKeyMappings());

    pianoRoll.setClip (&project->songClip);
    styleEditor.setPattern (&project->style);
    voiceEditor.setVoice (&project->voice);

    deviceToolbar.onMidiInputSelected = [this](const juce::String& dev){ engine.setMidiInput (dev); };
    deviceToolbar.onMidiOutputSelected= [this](const juce::String& dev){ engine.setMidiOutput (dev); };
    deviceToolbar.onTempoChanged     = [this](double bpm){ engine.setTempoBPM (bpm); };
    deviceToolbar.onTimeSigChanged   = [this](int n, int d){ engine.setTimeSignature (n, d); };

    transportBar.onPlay = [this]
    {
        if (juce::MidiOutput::getAvailableDevices().isEmpty())
        {
            juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                "No MIDI Output", "Pick a MIDI Output in the top-left first.");
            return;
        }
        engine.play();
    };
    transportBar.onStop   = [this]{ engine.stop(); };
    transportBar.onRecord = [this]{ engine.toggleRecord(); };


    pianoRoll.onAddNote = [this](const MidiNote& n){
        undo.perform (new AddNoteAction (project->songClip, n)); repaint();
    };
    pianoRoll.onResizeNote = [this](int idx, double newLen){
        if (idx >= 0 && idx < (int)project->songClip.notes.size())
        {
            auto oldLen = project->songClip.notes[(size_t)idx].lengthBeats;
            if (std::abs (oldLen - newLen) > 1e-6)
                undo.perform (new ResizeLastNoteAction (project->songClip, idx, oldLen, newLen));
            repaint();
        }
    };
    pianoRoll.onDeleteNote = [this](int idx){
        undo.perform (new DeleteNoteAction (project->songClip, idx)); repaint();
    };


    styleEditor.onToggleCell = [this](int r, int c){
        undo.perform (new ToggleCellAction (project->style, r, c)); repaint();
    };

    styleEditor.onAudition = [this]{ engine.auditionStyle(); };
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black.withAlpha (0.95f));
}
void MainComponent::resized()
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop (kTopBarHeight);
    deviceToolbar.setBounds (top.removeFromLeft (r.getWidth() * 2 / 3));
    transportBar.setBounds (top);
    tabs.setBounds (r.reduced (6));
}

void MainComponent::getAllCommands (juce::Array<juce::CommandID>& c)
{
    c.addArray ({
        CommandIDs::NewProject, CommandIDs::OpenProject, CommandIDs::SaveProject, CommandIDs::SaveProjectAs,
        CommandIDs::Play, CommandIDs::Stop,
        CommandIDs::ExportSMF, CommandIDs::ImportSMF, CommandIDs::Quantize,
        CommandIDs::Undo, CommandIDs::Redo
    });
}
void MainComponent::getCommandInfo (juce::CommandID id, juce::ApplicationCommandInfo& info)
{
    using juce::ModifierKeys;
    switch (id)
    {
        case CommandIDs::NewProject:   info.setInfo ("New Project", "Create a project", "File", 0); info.addDefaultKeypress ('n', ModifierKeys::commandModifier); break;
        case CommandIDs::OpenProject:  info.setInfo ("Open...", "Open a project", "File", 0); info.addDefaultKeypress ('o', ModifierKeys::commandModifier); break;
        case CommandIDs::SaveProject:  info.setInfo ("Save", "Save project", "File", 0); info.addDefaultKeypress ('s', ModifierKeys::commandModifier); break;
        case CommandIDs::SaveProjectAs:info.setInfo ("Save As...", "Save as", "File", 0); break;
        case CommandIDs::Play:         info.setInfo ("Play", "Start playback", "Transport", 0); info.addDefaultKeypress (juce::KeyPress::spaceKey, 0); break;
        case CommandIDs::Stop:         info.setInfo ("Stop", "Stop", "Transport", 0); break;
        case CommandIDs::ExportSMF:    info.setInfo ("Export MIDI...", "Export song as SMF", "File", 0); break;
        case CommandIDs::ImportSMF:    info.setInfo ("Import MIDI...", "Import SMF into song", "File", 0); break;
        case CommandIDs::Quantize:     info.setInfo ("Quantize 1/16", "Quantize to 1/16", "Edit", 0); info.addDefaultKeypress ('q', ModifierKeys::commandModifier); break;
        case CommandIDs::Undo:         info.setInfo ("Undo", "", "Edit", 0); info.addDefaultKeypress ('z', ModifierKeys::commandModifier); break;
        case CommandIDs::Redo:         info.setInfo ("Redo", "", "Edit", 0); info.addDefaultKeypress ('z', ModifierKeys::commandModifier | ModifierKeys::shiftModifier); break;
    }
}
bool MainComponent::perform (const juce::ApplicationCommandTarget::InvocationInfo& inv)
{
    switch (inv.commandID)
    {
        case CommandIDs::NewProject: newProject(); return true;
        case CommandIDs::OpenProject: openProject(); return true;
        case CommandIDs::SaveProject: saveProject (false); return true;
        case CommandIDs::SaveProjectAs: saveProject (true); return true;
        case CommandIDs::Play: engine.play(); return true;
        case CommandIDs::Stop: engine.stop(); return true;
        case CommandIDs::ExportSMF: exportSMF(); return true;
        case CommandIDs::ImportSMF: importSMF(); return true;
        case CommandIDs::Quantize: quantizeClip(); return true;
        case CommandIDs::Undo: undo.undo(); repaint(); return true;
        case CommandIDs::Redo: undo.redo(); repaint(); return true;
        default: break;
    }
    return false;
}

void MainComponent::newProject()
{
    project = std::make_unique<Project>();
    engine.setProject (project.get());
    pianoRoll.setClip (&project->songClip);
    styleEditor.setPattern (&project->style);
    voiceEditor.setVoice (&project->voice);
    undo.clearUndoHistory();
    repaint();
}

void MainComponent::openProject()
{
    juce::FileChooser chooser ("Open DMI Project", {}, "*.dmi.json");
    chooser.launchAsync (juce::FileBrowserComponent::openMode,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                if (auto loaded = Persist::loadProjectFromFile (file))
                {
                    project = std::move (loaded);
                    engine.setProject (project.get());
                    pianoRoll.setClip (&project->songClip);
                    styleEditor.setPattern (&project->style);
                    voiceEditor.setVoice (&project->voice);
                    undo.clearUndoHistory();
                    repaint();
                }
            }
        });
}

void MainComponent::saveProject (bool saveAs)
{
    auto doSave = [this](juce::File file)
    {
        if (file.getFileExtension().isEmpty())
            file = file.withFileExtension (".dmi.json");
        Persist::saveProjectToFile (*project, file);
        project->file = file.getFullPathName();
    };

    if (project->file.isEmpty() || saveAs)
    {
        juce::FileChooser chooser ("Save DMI Project", {}, "*.dmi.json");
        chooser.launchAsync (juce::FileBrowserComponent::saveMode,
            [this, doSave](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file != juce::File())
                    doSave (file);
            });
    }
    else
    {
        doSave (juce::File (project->file));
    }
}

void MainComponent::exportSMF()
{
    juce::FileChooser chooser ("Export MIDI File", {}, "*.mid");
    chooser.launchAsync (juce::FileBrowserComponent::saveMode,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file != juce::File())
                SMF::exportClipToSMF (project->songClip, file.withFileExtension (".mid"));
        });
}

void MainComponent::importSMF()
{
    juce::FileChooser chooser ("Import MIDI File", {}, "*.mid");
    chooser.launchAsync (juce::FileBrowserComponent::openMode,
        [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
                if (auto clip = SMF::importClipFromSMF (file))
                { project->songClip = std::move (*clip); repaint(); }
        });
}

void MainComponent::quantizeClip()
{
    engine.quantizeClip (project->songClip, 0.25 /* 1/16 beat */, 1.0);
    repaint();
}
