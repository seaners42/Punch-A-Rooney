#pragma once

#include <filesystem.h>
#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>
#include <cmath>

#include "enums.h"
#include "layouts.h"
#include "multiplayer.h"
#include "save.h"
#include "transition.h"
#include "usefulfunctions.h"

#include "maxmod9.h"
#include "soundbank.h"

// I give credit to William278 because most of how the menu navigation works comes from his Breaking
// Bad DS game.

// Not everything was directly copied, but he's the reason I used a similar header file for my
// layouts

class Menu
{
  // Standard Menu & Navigation

  Menus currentMenu = NO_MENU;
  Menus previousMenu = NO_MENU;
  Layout* currentLayout = &LAYOUTS[0];
  MenuSelection currentSelection = NONE;
  int selectionRow = -1;
  int touchLast[2] = {0, 0};
  bool doubleCheck = false;

  // Song info
  mm_word menuSong;
  int chosenSong = 1;
  mm_word mus_player[10] = {MOD_TITLETHEME,  MOD_SLIMJIM,         MOD_JILL,   MOD_PABLO,
                            MOD_SVETLANA,    MOD_KAIRO,           MOD_ROONEY, MOD_INTERMISSION,
                            MOD_CHAMPSCREEN, MOD_UNDISPUTEDSCREEN};
  const char* song_names[10] = {"Title",     "Slim Jim",      "Fra-Jill",    "Lil Pablo",
                                "Hailfist",  "The Kommondor", "Punchstache", "Cornerman",
                                "Champion!", "Undefeated!"};
  bool songPlaying = true;

  // Page navigation
  int pageNumber = 1;
  int Cursor = 0;

  // Custom Game
  bool titleLoaded = true;

  const int customValues[6][3] = {
      {0, 3, 1}, {1, 5, 1}, {1, 3, 1},

      {0, 5, 1}, {0, 5, 1}, {0, 5, 1},
  };

  // Defined on config screen
  int customFlags[6] = {
      1, 3, 2,

      0, 0, 0,
  };

  // Custom game info
  const char* customNames[3] = {
      "Difficulty",
      "Rounds",
      "Timer Speed",
  };

  const char* difficultyTexts[4] = {
      "No AI",
      "Rookie",
      "Normal",
      "Champ",
  };

  const char* speedTexts[3] = {"Actual", "Normal", "Faster"};

  const char* boxersText[6] = {"Rooney", "Slim Jim", "Jill", "Lil Pablo", "Svetlana", "Kairo"};

  const char* boxersBgs[6] = {
      "bg/Partial0", "bg/Partial1", "bg/Partial2", "bg/Partial3", "bg/Partial4", "bg/Partial5",
  };

  const char* galleryBgs[7] = {
      "bg/Gallery1", "bg/Gallery2", "bg/Gallery3", "bg/Gallery4",
      "bg/Gallery5", "bg/Gallery6", "bg/Gallery7",
  };

public:
  Menu();
  ~Menu();

  int Load(Menus menu);

  MenuSelection handleInput();
  MenuSelection handleLayoutInput(touchPosition touch);

  bool isTouchOnButton(int row, touchPosition touch, bool up);
  bool isTouchInBounds(touchPosition touch, int x1, int y1, int x2, int y2, bool up);

  void DrawLayout();
  void UpdateLayout();

  void buttonInterpret();

  void setMenu(Menus menu);

  void showFreeplayMaps(bool showSprites);
  void updateFreeplayMaps();

  void showFreeplayBoxers(bool showSprites);
  void updateFreeplayBoxers();

  void showGuide(bool showSprites);
  void updateGuide();

  void showGallery(bool showSprites);

  void showRadio(bool showSprites);

  void showRecordsBoxers(bool showSprites);
  void updateRecordsBoxers();

  int* getCustomGame();

  int Update();

  void Unload();
};