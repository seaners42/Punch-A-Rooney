#pragma once

// Should've had a lot more in here but it's okay

// Global tick

inline int tick = 0;

// Favorite color grabber

int const static themes[16][3] = {
    {12, 16, 19}, {23, 23, 0}, {30, 0, 3},  {30, 17, 30},

    {30, 18, 0},  {29, 28, 0}, {21, 30, 0}, {0, 30, 0},

    {0, 20, 7},   {9, 27, 17}, {6, 23, 39}, {0, 11, 29},

    {0, 0, 18},   {17, 0, 26}, {26, 0, 29}, {30, 0, 18},

};

auto myFavColor(int theme, int value) -> int;

// lerp

auto lerp(float a, float b, float t) -> float;
