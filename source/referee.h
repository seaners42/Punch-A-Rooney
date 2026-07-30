#pragma once

#include <filesystem.h>
#include <math.h>
#include <maxmod9.h>
#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>

#include "boxer.h"
#include "camera.h"
#include "enums.h"
#include "maxmod9.h"
#include "mm_types.h"
#include "soundbank.h"
#include "usefulfunctions.h"

class Referee
{
public:
  // Position

  int m_x = 0;
  int m_y = 100;

  int midpoint = 96;
  ;

  // Change x and y by

  int m_dx = 0;
  int m_dy = 0;
  int m_maxSpeed = 1;

  // Current movement state

  Movement m_movement = STILL;
  Movement m_looking = LEFT;

  // Classification

  int m_index;

  // Animation Variables

  int m_intClock = 0;
  int m_indieClock = 0;
  int m_frame = 0;

  // Gameplay Variables

  RefMoves m_state = REFIDLE;
  bool askedToBox;
  int timeSpentCounting;
  bool makingDecision = false;

  bool allowMovement = true;

  mm_word count[10] = {SFX_REF_ONE, SFX_REF_TWO,   SFX_REF_THREE, SFX_REF_FOUR, SFX_REF_FIVE,
                       SFX_REF_SIX, SFX_REF_SEVEN, SFX_REF_EIGHT, SFX_REF_NINE, SFX_REF_TEN};

  Referee() {}
  ~Referee();

  Referee(int index);

  int load();
  void resetPosition();
  void unload();

  void hide(bool show);

  void update(Boxer& boxer1, Boxer& boxer2, bool roundActive, bool matchActive);

  void decisionSequence(int expectedWinner, bool onlyFirstFrame);

  void animationHandler(Boxer& boxer1, Boxer& boxer2, bool matchActive);
  void movementHandler(Boxer& boxer1, Boxer& boxer2, bool roundActive, bool matchActive);
  void cameraHandler(int& screenX, int& screenY);
};