#include <JuceHeader.h>
#include "StylePatternEditor.h"

StylePatternEditor::StylePatternEditor()
{
    addAndMakeVisible (auditionBtn);
    auditionBtn.onClick = [this]{ if (onAudition) onAudition(); };
}
void StylePatternEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0x141618));
    if (!pattern) return;

    auto r = getLocalBounds().reduced (8);
    auto cellW = r.getWidth() / cols;
    auto cellH = (r.getHeight() - 36) / rows;

    g.setColour (juce::Colour (0x26292d));
    for (int c=0;c<=cols;++c) g.drawVerticalLine (r.getX()+c*cellW, r.getY(), r.getBottom()-36.0f);
    for (int rr=0;rr<=rows;++rr) g.drawHorizontalLine (r.getY()+rr*cellH, r.getX(), r.getRight());

    for (int rr=0; rr<rows; ++rr)
        for (int c=0; c<cols; ++c)
            if (pattern->grid[rr][c])
            {
                juce::Rectangle<float> cell (r.getX()+c*cellW+2, r.getY()+rr*cellH+2, cellW-4, cellH-4);
                g.setColour (juce::Colour (0xff8fff7a));
                g.fillRoundedRectangle (cell, 3.0f);
            }
}
void StylePatternEditor::resized()
{
    auditionBtn.setBounds (getLocalBounds().removeFromBottom (32).reduced (8));
}
void StylePatternEditor::mouseDown (const juce::MouseEvent& e)
{
    if (!pattern) return;
    auto r = getLocalBounds().reduced (8);
    auto cellW = r.getWidth() / cols;
    auto cellH = (r.getHeight() - 36) / rows;
    if (! r.contains (e.position.toInt())) return;

    int c = juce::jlimit (0, cols-1, (e.x - r.getX()) / cellW);
    int rr= juce::jlimit (0, rows-1, (e.y - r.getY()) / cellH);

    if (onToggleCell) onToggleCell (rr, c);
    repaint();
}
