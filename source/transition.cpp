#include "transition.h"

void Transition(int screen, int start, int end, bool lock)
{
  brightness = start;
  endBrightness = end;
  lockInput = lock;
  m_screenToTransition = screen;
  setBrightness(m_screenToTransition, brightness);
  isTransitioning = true;
}

void TransitionUpdate(int clock)
{
  if (isTransitioning)
  {
    if (clock % 2 == 0)
    {
      setBrightness(m_screenToTransition, brightness);
      if (brightness == endBrightness)
      {
        isTransitioning = false;
        lockInput = false;
      }
      else
        brightness += (brightness < endBrightness) ? 1 : -1;
    }
  }
}

bool getLock()
{
  return lockInput;
}

void setLock(bool param)
{
  lockInput = param;
}