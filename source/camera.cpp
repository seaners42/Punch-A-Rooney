#include "camera.h"

void worldToScreen(int fWorldX, int fWorldY, int& nScreenX, int& nScreenY, int size)
{
  nScreenX = (int)((fWorldX - fOffsetX) * fScale) - (int)((size / 2.0f) - fScale);
  nScreenY = (int)((fWorldY - fOffsetY) * fScale) - (int)((size / 2.0f) - fScale);
}

void screenToWorld(int nScreenX, int nScreenY, int& fWorldX, int& fWorldY)
{
  fWorldX = (int)(((nScreenX) / fScale) + fOffsetX);
  fWorldY = (int)(((nScreenY) / fScale) + fOffsetY);
}