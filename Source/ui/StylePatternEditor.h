#pragma once
#include <JuceHeader.h>
#include "../model/StylePattern.h"

class StylePatternEditor : public juce::Component
{
public:
    StylePatternEditor();
    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;

    void setPattern (StylePattern* p) { pattern = p; repaint(); }
    std::function<void()> onAudition;
    std::function<void(int row, int col)> onToggleCell;

private:
    StylePattern* pattern = nullptr;
    int rows = 8;
    int cols = 16;
    juce::TextButton auditionBtn { "Audition" };
};
