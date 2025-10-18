#pragma once
#include <JuceHeader.h>
#include "../model/MidiClip.h"
#include "../model/StylePattern.h"

struct AddNoteAction : public juce::UndoableAction
{
    MidiClip& clip; MidiNote note; int insertedIndex = -1;
    AddNoteAction (MidiClip& c, MidiNote n) : clip (c), note (n) {}
    bool perform() override { clip.notes.push_back (note); insertedIndex = (int)clip.notes.size()-1; return true; }
    bool undo() override    { if (insertedIndex >=0 && insertedIndex < (int)clip.notes.size()) clip.notes.erase (clip.notes.begin()+insertedIndex); return true; }
    int getSizeInUnits() override { return 1; }
};

struct ResizeLastNoteAction : public juce::UndoableAction
{
    MidiClip& clip; int index; double before{}, after{};
    ResizeLastNoteAction (MidiClip& c, int idx, double oldLen, double newLen) : clip (c), index (idx), before (oldLen), after (newLen) {}
    bool perform() override { if (index>=0 && index<(int)clip.notes.size()) clip.notes[(size_t)index].lengthBeats = after; return true; }
    bool undo() override    { if (index>=0 && index<(int)clip.notes.size()) clip.notes[(size_t)index].lengthBeats = before; return true; }
    int getSizeInUnits() override { return 1; }
};

struct DeleteNoteAction : public juce::UndoableAction
{
    MidiClip& clip; MidiNote note; int index;
    DeleteNoteAction (MidiClip& c, int idx) : clip (c), index (idx) { note = clip.notes[(size_t)idx]; }
    bool perform() override { if (index>=0 && index<(int)clip.notes.size()) clip.notes.erase (clip.notes.begin()+index); return true; }
    bool undo() override    { if (index>=0 && index<=(int)clip.notes.size()) clip.notes.insert (clip.notes.begin()+index, note); return true; }
    int getSizeInUnits() override { return 1; }
};

struct ToggleCellAction : public juce::UndoableAction
{
    StylePattern& patt; int r, c; bool prev{};
    ToggleCellAction (StylePattern& p, int row, int col) : patt (p), r (row), c (col), prev (p.grid[row][col]) {}
    bool perform() override { patt.grid[r][c] = !prev; return true; }
    bool undo() override    { patt.grid[r][c] =  prev; return true; }
    int getSizeInUnits() override { return 1; }
};
