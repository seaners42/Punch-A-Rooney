#pragma once

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

#include <fat.h>
#include <nds.h>

#include <nds/arm9/dldi.h>

typedef struct
{
  bool playedOnce;
  bool lookedAtGuide;

  bool wonOnce;
  bool wonTDOnce;

  int storyProgress;
  int TDProgress;

  int storyTimes[10];
  int TDTimes[10];

  int recordTimes[10];
  int recordTD[10];
  int recordMultiplayer[3][6];

  bool muteMusic;

} stored_data;

inline stored_data game_data;
inline bool saving_possible = true;

bool load_data(const char* path);

bool save_data(const char* path);

void reset_data();