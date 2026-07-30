#pragma once

#include <filesystem.h>
#include <math.h>
#include <maxmod9.h>
#include <nds.h>
#include <nf_lib.h>

#include <algorithm>

#include "camera.h"
#include "characters.h"
#include "debug.h"
#include "enums.h"
#include "soundbank.h"
#include "transition.h"
#include "usefulfunctions.h"

class Boxer
{
public:
  // Position

  int m_x = 0;
  int m_y = 100;
  int m_rot = 0;

  // Gameplay Variables

  int m_hp = 150;
  int m_stamina = 100;
  int m_block = 40;
  int m_super = 0;
  Moves m_state = IDLE;

  int m_downs = 0;
  int m_rounddowns = 0;
  int m_tireouts = 0;

  // Change x and y by

  int m_dx = 0;
  int m_dy = 0;
  int m_maxSpeed = 1;

  bool m_juked = false;
  bool m_boxerLocked = false;

  Character m_character;

  // Animation Variables

  int m_intClock = 0;
  int m_indieClock = 0;
  int m_frame = 0;

  // no pun intended
  int m_lightswitch = false;

  // Get Up Count handles if you get up after a knockdown

  int m_getUpCount = 0;

  // Current movement state

  Movement m_movement = STILL;

  // Assignment

  bool m_isPlayerOne = false;
  int m_aiLevel = 0;
  bool m_npc = false;
  bool m_titleDefense;

  // Classification

  traits* m_traits = {};
  superSequence* m_superSeq = {};
  int m_index;

  // Statistics

  int m_punchesThrown;
  int m_punchesLanded;

  // Handles action speed

  int m_punchFreq = 6;
  int jukedPunchFreq = 18;

  // Cooldowns
  bool m_punchedAlready = false;
  bool m_damagedAlready = false;
  int m_punchWait = 0;
  int m_blockWait = 0;
  int m_staminaWait = 0;
  int m_shieldWait = 0;
  int m_comboWindow;
  int m_lastDamaged;

  // Action allow states

  bool m_canPunch = true;
  bool m_canBlock = true;
  bool m_canUnblock = true;

  // Super manager

  int m_sequenceSuper;
  int mp_sequenceSuper;
  bool m_superActive = false;

  // Champion should have some fun

  bool m_kairoHavingFun = false;

  const char* boxerPaths[12] = {
      "sprite/rooney",         "sprite/slim_jim",       "sprite/jill",
      "sprite/lilpablo",       "sprite/svetlana",       "sprite/kairo",
      "sprite/rooney_champ",   "sprite/slim_jim_champ", "sprite/jill_champ",
      "sprite/lilpablo_champ", "sprite/svetlana_champ", "sprite/kairo_champ",
  };

  int sfxhurt1;
  int sfxhurt2;
  int sfxdown;
  int sfxwin;

private:
  // nothing

public:
  Boxer() {}
  Boxer(Character character, int cpu, int index, bool isPlayer1, bool titleDefense);
  ~Boxer();

  int load();
  void resetPosition();
  void unload();

  void hide(bool show);

  void handleInput();
  void damage(int dmg, int knock, bool superLock, bool hurts);
  void unlock();

  void update(Boxer& opponent, bool roundActive, bool matchActive);

  void combatHandler(Boxer& opponent);
  void superHandler(Boxer& opponent, bool roundActive);

  void animationHandler(bool matchActive, Boxer& opponent);
  void npcHandler(int lvl, Boxer& opponent);

  bool confirmAction(Moves state, Boxer& opponent, bool checkingself);
  bool hitPunch(Moves state, Boxer& opponent, bool flip, bool doDamage);
  void missedPunch(Moves state, Boxer& opponent, bool flip);

  void movementHandler(Boxer& opponent, bool roundActive, bool matchActive);
  void downHandler(bool mp);
  void killAi();
  void cameraHandler(int& screenX, int& screenY);

  int getStatistics(int index);
};