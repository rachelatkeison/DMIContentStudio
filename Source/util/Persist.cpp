#include <JuceHeader.h>
#include "Persist.h"

static juce::var noteToVar (const MidiNote& n)
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty ("pitch", n.pitch);
    obj->setProperty ("vel",   n.velocity);
    obj->setProperty ("start", n.startBeats);
    obj->setProperty ("len",   n.lengthBeats);
    return obj.get();
}
static MidiNote varToNote (const juce::var& v)
{
    MidiNote n;
    auto* o = v.getDynamicObject();
    n.pitch = (int) o->getProperty ("pitch");
    n.velocity = (int) o->getProperty ("vel");
    n.startBeats = (double) o->getProperty ("start");
    n.lengthBeats= (double) o->getProperty ("len");
    return n;
}

juce::var Persist::toVar (const Project& p)
{
    juce::DynamicObject::Ptr root = new juce::DynamicObject();
    root->setProperty ("name", p.name);
    root->setProperty ("tempo", p.tempoBPM);
    root->setProperty ("tsN", p.timeSigNum);
    root->setProperty ("tsD", p.timeSigDen);

    juce::Array<juce::var> notes;
    for (auto& n : p.songClip.notes) notes.add (noteToVar (n));
    juce::DynamicObject::Ptr clip = new juce::DynamicObject();
    clip->setProperty ("length", p.songClip.lengthBeats);
    clip->setProperty ("notes", notes);
    root->setProperty ("clip", clip.get());

    juce::Array<juce::var> grid;
    for (int r=0;r<StylePattern::rows;++r)
        for (int c=0;c<StylePattern::cols;++c)
            grid.add ((bool) p.style.grid[r][c]);
    root->setProperty ("styleGrid", grid);

    juce::DynamicObject::Ptr voice = new juce::DynamicObject();
    voice->setProperty ("attack", p.voice.attack);
    voice->setProperty ("decay", p.voice.decay);
    voice->setProperty ("sustain", p.voice.sustain);
    voice->setProperty ("release", p.voice.release);
    voice->setProperty ("cutoff", p.voice.cutoff);
    voice->setProperty ("res", p.voice.res);
    voice->setProperty ("vibrato", p.voice.vibrato);
    voice->setProperty ("velCurve", p.voice.velCurve);
    voice->setProperty ("program", p.voice.programNumber);
    voice->setProperty ("channel", p.voice.channel);
    root->setProperty ("voice", voice.get());

    return root.get();
}
void Persist::fromVar (Project& p, const juce::var& v)
{
    auto* root = v.getDynamicObject();
    p.name = root->getProperty ("name").toString();
    p.tempoBPM = (double) root->getProperty ("tempo");
    p.timeSigNum = (int) root->getProperty ("tsN");
    p.timeSigDen = (int) root->getProperty ("tsD");

    if (auto* clip = root->getProperty ("clip").getDynamicObject())
    {
        p.songClip.lengthBeats = (double) clip->getProperty ("length");
        if (auto arr = clip->getProperty ("notes").getArray())
        {
            p.songClip.notes.clear();
            for (auto& it : *arr) p.songClip.notes.push_back (varToNote (it));
        }
    }
    if (auto arr = root->getProperty ("styleGrid").getArray())
    {
        int i=0;
        for (int r=0;r<StylePattern::rows;++r)
            for (int c=0;c<StylePattern::cols;++c)
                p.style.grid[r][c] = (bool) (*arr)[i++];
    }
    if (auto* voice = root->getProperty ("voice").getDynamicObject())
    {
        p.voice.attack  = (int) voice->getProperty ("attack");
        p.voice.decay   = (int) voice->getProperty ("decay");
        p.voice.sustain = (int) voice->getProperty ("sustain");
        p.voice.release = (int) voice->getProperty ("release");
        p.voice.cutoff  = (int) voice->getProperty ("cutoff");
        p.voice.res     = (int) voice->getProperty ("res");
        p.voice.vibrato = (int) voice->getProperty ("vibrato");
        p.voice.velCurve= (int) voice->getProperty ("velCurve");
        p.voice.programNumber = (int) voice->getProperty ("program");
        p.voice.channel       = (int) voice->getProperty ("channel");
    }
}
std::unique_ptr<Project> Persist::loadProjectFromFile (const juce::File& f)
{
    auto json = juce::JSON::parse (f);
    if (json.isVoid()) return nullptr;
    auto p = std::make_unique<Project>();
    fromVar (*p, json);
    p->file = f.getFullPathName();
    return p;
}
bool Persist::saveProjectToFile (const Project& p, const juce::File& f)
{
    auto json = juce::JSON::toString (toVar (p), true);
    return f.replaceWithText (json);
}
