#include "usefulfunctions.h"

int myFavColor(int theme, int value)
{
  return themes[theme][value];
}

float lerp(float a, float b, float t)
{
  return a + t * (b - a);
}
