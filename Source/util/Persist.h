#pragma once
#include <JuceHeader.h>
#include "../model/Project.h"

struct Persist
{
    static std::unique_ptr<Project> loadProjectFromFile (const juce::File& f);
    static bool saveProjectToFile (const Project& p, const juce::File& f);

private:
    static juce::var toVar (const Project& p);
    static void fromVar (Project& p, const juce::var& v);
};
