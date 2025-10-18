#pragma once
#include <JuceHeader.h>

struct StylePattern
{
    static constexpr int rows = 8;
    static constexpr int cols = 16;
    bool grid[rows][cols] {};

    int channelForRow (int row) const { return (row == 0 ? 10 : 1); }
    int pitchForCell (int row, int /*col*/) const
    {
        static const int map[rows] = {36, 38, 42, 46, 35, 40, 48, 50};
        return map[row % rows];
    }
};
