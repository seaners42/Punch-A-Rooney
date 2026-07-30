#pragma once

// Camera Variables

inline int fOffsetX = 0;
inline int fOffsetY = 0;
inline int fZoomCenterX = 128;
inline int fZoomCenterY = 96;
inline float fScale = 1.0f;

void worldToScreen(int fWorldX, int fWorldY, int& nScreenX, int& nScreenY, int size);

void screenToWorld(int nScreenX, int nScreenY, int& fWorldX, int& fWorldY);