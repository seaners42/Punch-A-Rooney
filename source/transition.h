#pragma once

#include <nds.h>

inline bool isTransitioning = false;
inline bool lockInput = false;
inline int brightness = 0;
inline int m_screenToTransition;
inline int endBrightness;

void Transition(int screen, int start, int end, bool lock);
void TransitionUpdate(int clock);
