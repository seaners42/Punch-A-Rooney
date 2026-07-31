#include <filesystem.h>
#include <math.h>
#include <maxmod9.h>
#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>
#include <iostream>

#include "boxer.h"
#include "camera.h"
#include "debug.h"
#include "dialogue.h"
#include "enums.h"
#include "matchinfo.h"
#include "menu.h"
#include "mm_types.h"
#include "multiplayer.h"
#include "referee.h"
#include "save.h"

#include "soundbank.h"
#include "transition.h"
#include "usefulfunctions.h"

class Game
{
  char* save_path = {};

  int currentScene = INTRO;
  bool isRunning = false;
  bool paused = false;
  bool boxersLoaded = false;

  Boxer boxer1;
  Boxer boxer2;
  Referee ref;
  Match* currentMatch = {};
  GameType currentGame;

  mm_word matchMusic;

  int storyCounter;

  bool isPlayerOne = true;

  // Menu Stuff
  Menu menu;

  // Game Variables
  bool titleDefense = false;

  int whenToStart;
  int maxRounds;
  int timerSpeed;
  int* customValues = {};

  // Sequence Counters (Method varies by convenience)

  int intromove = 0;
  int sequenceMatchOver;
  int sequenceInterval = 999;
  int sequenceDownCount;
  int sequenceSuper;

  // Round transition check

  bool transitioningIntoInterval = false;
  bool interval = false;

  // Basic match states

  bool roundActive;
  bool matchActive;

  // Timer for round and time spent knocked down

  int bellAnim = 0;
  int roundTimer;
  int downTimer;

  int roundNumber;

  // Down Handlers

  bool boxerDown;
  bool playerDowned;

  // Custom Game
  bool multiplayerReady;

  // 1 for boxer 1, 2 for boxer 2

  int expectedWinner;
  int makingDecision;

  // Crowd Stuff

  int crowdVolume = 0;
  int crowdStart = 0;
  mm_sfxhand crowd;

  // congrats dude

  bool showCredits;

  // Interval Stuff

  const char* timeoutBoxer[12] = {
      "bg/TimeoutRooney",      "bg/TimeoutSlimJim",       "bg/TimeoutJill",
      "bg/TimeoutPablo",       "bg/TimeoutSvetlana",      "bg/TimeoutKairo",
      "bg/TimeoutRooneyChamp", "bg/TimeoutSlimJimChamp",  "bg/TimeoutJillChamp",
      "bg/TimeoutPabloChamp",  "bg/TimeoutSvetlanaChamp", "bg/TimeoutKairoChamp"};

  const char stageinformation[6][4][15] = {
      {
          "bg/Arena1",
          "bg/Gym",
          "bg/Bottom1",
          "bg/Timeout1",
      },
      {
          "bg/Arena2",
          "bg/Audience",
          "bg/Bottom2",
          "bg/Timeout2",
      },
      {
          "bg/Arena3",
          "bg/Audience",
          "bg/Bottom3",
          "bg/Timeout3",
      },
      {
          "bg/Arena4",
          "bg/Audience",
          "bg/Bottom4",
          "bg/Timeout4",
      },
      {
          "bg/Arena5",
          "bg/Audience",
          "bg/Bottom5",
          "bg/Timeout5",
      },
      {
          "bg/Arena6",
          "bg/Audience",
          "bg/Bottom6",
          "bg/Timeout6",
      },
  };

  const mm_word stageMusic[6] = {MOD_ROONEY, MOD_SLIMJIM,  MOD_JILL,
                                 MOD_PABLO,  MOD_SVETLANA, MOD_KAIRO};

  const int stagecolor[6][3] = {{13, 13, 15}, {15, 3, 3},   {7, 11, 12},
                                {8, 10, 19},  {27, 27, 25}, {11, 14, 19}};

public:
  Game();
  ~Game();

  void initializeDS(int argc, char** argv);

  // debug does nothing rn
  void setup2DGraphics(bool debug);

  int loadBoxingRing(GameType gameType, int id);
  void inInterval(bool show);
  void unloadBoxingRing();

  int loadPreviewStory();
  void unloadPreviewStory();

  int loadWinScreen(bool undisputed);
  void unloadWinScreen();

  void crowdSounds();

  // "when" how many ticks from now
  void newRound(int when, bool updater);
  void decision(bool updater);

  void loadFightTop();
  void updateFightTop();
  void unloadFightTop();

  void loadFightBottom();
  void updateFightBottom();
  void unloadFightBottom();

  void intervalTransition(bool transitioning_in, bool updater);
  bool endGame();

  void startIntro();
  void unloadIntro();
  void startMainMenu(bool intro);

  void handleEvents();
  int manageFight();
  void cameraHandler();

  void update();
  void render();

  bool running()
  {
    return isRunning;
  }
};