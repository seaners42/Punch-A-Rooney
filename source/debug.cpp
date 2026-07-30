#include "debug.h"

// Routine taken from https://devkitpro.org/viewtopic.php?t=3057

int getMemUsed()
{ // returns the amount of used memory in bytes
  struct mallinfo mi = mallinfo();
  return mi.uordblks;
}

int getMemFree()
{ // returns the amount of free memory in bytes
  struct mallinfo mi = mallinfo();
  return mi.fordblks + (getHeapLimit() - getHeapEnd());
}