#include "game.h"
#include "enums.h"
#include "menu.h"
#include "mm_types.h"
#include "multiplayer.h"
#include "nds/arm9/video.h"
#include "nds/arm9/window.h"
#include "nds/debug.h"
#include "nds/system.h"
#include "save.h"
#include "transition.h"
#include <cmath>

Game::Game() {}
Game::~Game() {}

void Game::initializeDS(int argc, char** argv)
{
  // Prepare a NitroFS initialization screen
  consoleDemoInit();
  printf("Let's get ready to rumble...\n");

  sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);
  while (REG_VCOUNT < 80)
    ;

  if (!nitroFSInit(NULL))
  {
    printf("\x1B[31mFailed to start nitroFS\n");
    printf("\x1B[31mPlease reset the system\n");

    videoSetMode(MODE_5_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    isRunning = false;

    while (1)
    {
      swiWaitForVBlank();
    }
  }

  // Init Wifi
  printf("Initializing WiFi...\n");

  if (!Wifi_InitDefault(INIT_ONLY | WIFI_LOCAL_ONLY))
  {
    printf("Can't initialize WiFi!\n");
    while (1)
    {
      swiWaitForVBlank();
    }
  }

  disable_multiplayer();

  swiWaitForVBlank();

  // Initialize NitroFS and set it as the root folder of the filesystem
  if (nitroFSInit(NULL))
  {
    // do nothing
  }
  NF_SetRootFolder("NITROFS");

  bool init_ok = fatInitDefault();
  if (!init_ok)
  {
    // We need FAT access for this example
    perror("fatInitDefault()");
    saving_possible = false;
    printf("Continuing without save data\n");
  }

  // Only ever do this stuff if you are on a system that allows save data (i.e. DS Hardware or
  // Emulator)

  if (saving_possible)
  {
    if (argc == 0)
    {
      printf("Error: argc = 0\n");
    }

    if (argv[0] == NULL)
    {
      printf("Error: argv[0] = NULL\n");
    }

    if (strlen(argv[0]) < 4)
    {
      printf("Error: argv[0] is too short\n");
    }

    // Create a string to hold the path to the saved data.

    save_path = strdup(argv[0]);
    if (save_path == NULL)
    {
      printf("Not enough memory for strdup()\n");
    }

    // Replace the ".nds" extension by ".sav"
    size_t save_path_len = strlen(save_path);
    strcpy(&save_path[save_path_len - 3], "sav");

    printf("Save path: %s\n", save_path);
  }

  // Try to load saved data

  if (load_data(save_path))
  {
    nocashMessage("Saved data loaded!\n");
  }
  else
  {
    // Create new save data for a first timer

    game_data.playedOnce = false;
    game_data.lookedAtGuide = false;

    game_data.wonOnce = false;
    game_data.wonTDOnce = false;

    game_data.storyProgress = 1;
    game_data.TDProgress = 6;

    game_data.muteMusic = false;

    for (int i = 0; i < 10; i++)
    {
      game_data.storyTimes[i] = -1;
    }
    for (int i = 0; i < 10; i++)
    {
      game_data.TDTimes[i] = -1;
    }
    for (int i = 0; i < 10; i++)
    {
      game_data.recordTimes[i] = -1;
    }
    for (int i = 0; i < 10; i++)
    {
      game_data.recordTD[i] = -1;
    }
    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        game_data.recordMultiplayer[j][i] = 0;
      }
    }

    if (save_data(save_path))
    {
      nocashMessage("Created a new save!\n");
    }
    else
    {
      nocashMessage("Sorry bro, you gotta do like save states or smth!\n");
    }
  }

  // Init sound and music bank

  mmInitDefault("soundbank.bin");

  // Up and running baby

  isRunning = true;
  paused = false;
}

void Game::setup2DGraphics(bool debug)
{
  // Initialize 2D engine in both screens and use mode 0
  NF_Set2D(0, 2);
  NF_Set2D(1, 2);

  // Initialize tiled backgrounds system
  NF_InitTiledBgBuffers(); // Initialize storage buffers

  NF_InitAffineBgSys(0); // Top screen
  NF_InitTiledBgSys(1);  // Bottom screen

  // Initialize sprite system
  NF_InitSpriteBuffers(); // Initialize storage buffers
  NF_InitSpriteSys(0);    // Top screen

  NF_InitSpriteSys(1); // Bottom screen

  NF_InitTextSys(1);

  // Load assets from filesystem to RAM
  NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);
  NF_CreateTextLayer(1, 0, 0, "normal");

  NF_DefineTextColor(1, 0, 1, 31, 31, 28); // White
  NF_DefineTextColor(1, 0, 2, 31, 31, 10); // Yellow-Ish
  NF_DefineTextColor(1, 0, 3, 0, 3, 5);    // Black

  srand(time(NULL));
}

int Game::loadBoxingRing(GameType gameType, int id)
{

  // For FREE or MP
  customValues = menu.getCustomGame();

  // Switches between STORY_GAME, FREE_GAME, and MP_GAME (Career, Freeplay, and Local respectively)
  currentGame = gameType;

  // Story mode
  titleDefense = id > 5;
  currentMatch = GetMatch(id);
  storyCounter = id;

  // Freeplay
  int difficulies[5] = {
      -42,
      7,
      35,
      1000,
  };

  currentScene = GAME;

  // Handle loading based on game type
  switch (gameType)
  {
    case STORY_GAME:
      NF_LoadAffineBg(stageinformation[storyCounter - (titleDefense ? 5 : 0)][0], "Arena", 512,
                      512);
      NF_LoadAffineBg(stageinformation[storyCounter - (titleDefense ? 5 : 0)][1], "Background", 512,
                      512);
      NF_LoadAffineBg(stageinformation[storyCounter - (titleDefense ? 5 : 0)][2], "Bottom", 256,
                      256);

      NF_CreateAffineBg(0, 2, "Arena", 0);
      NF_CreateAffineBg(0, 3, "Background", 0);
      NF_CreateAffineBg(1, 2, "Bottom", 0);

      boxer1 = Boxer(ROONEY, 0, 0, !titleDefense, titleDefense);
      boxer2 = Boxer(currentMatch->storyOpponent, currentMatch->difficulty, 1, titleDefense,
                     titleDefense);
      ref = Referee(15);
      break;

    case FREE_GAME:

      NF_LoadAffineBg(stageinformation[customValues[3]][0], "Arena", 512, 512);
      NF_LoadAffineBg(stageinformation[customValues[3]][1], "Background", 512, 512);
      NF_LoadAffineBg(stageinformation[customValues[3]][2], "Bottom", 256, 256);

      NF_CreateAffineBg(0, 2, "Arena", 0);
      NF_CreateAffineBg(0, 3, "Background", 0);
      NF_CreateAffineBg(1, 2, "Bottom", 0);

      boxer1 = Boxer((Character)customValues[4], 0, 0, true, false);
      boxer2 = Boxer((Character)customValues[5], difficulies[customValues[0]], 1, false, false);
      ref = Referee(15);
      break;

    case MP_GAME:
      NF_LoadAffineBg(stageinformation[customValues[3]][0], "Arena", 512, 512);
      NF_LoadAffineBg(stageinformation[customValues[3]][1], "Background", 512, 512);
      NF_LoadAffineBg(stageinformation[customValues[3]][2], "Bottom", 256, 256);

      NF_CreateAffineBg(0, 2, "Arena", 0);
      NF_CreateAffineBg(0, 3, "Background", 0);
      NF_CreateAffineBg(1, 2, "Bottom", 0);

      boxer1 = Boxer((Character)customValues[isHost ? 4 : 5], 0, 0, isHost, false);
      boxer2 = Boxer((Character)customValues[!isHost ? 4 : 5], -1, 1, !isHost, false);
      ref = Referee(15);
      break;
  }

  boxer1.load();
  boxer2.load();
  ref.load();

  boxersLoaded = true;

  // Load the top and bottom screen assets
  loadFightTop();
  loadFightBottom();

  // All the game data
  roundActive = false;
  matchActive = true;
  roundTimer = 10800;
  whenToStart = (tick + 240);
  roundNumber = 1;
  makingDecision = 0;
  maxRounds = (currentGame != STORY_GAME) ? customValues[1] : 3;
  timerSpeed = (currentGame != STORY_GAME) ? customValues[2] : 2;

  // Stuff for the Interval scene (can we get props for the cornermen)
  interval = false;
  transitioningIntoInterval = false;

  // Crowd variables
  crowdStart = tick;
  crowdVolume = 0;

  // Audio temp, pitch, and volume. (Make it a little lower if you're on title defense for dramatic
  // effect)
  mmSetModuleTempo(1024);
  mmSetModulePitch(titleDefense ? 960 : 1024);
  mmSetModuleVolume(game_data.muteMusic ? 0 : 1024 / 3);

  matchMusic = (currentGame != STORY_GAME) ? stageMusic[customValues[3]]
                                           : stageMusic[storyCounter - (titleDefense ? 5 : 0)];

  // SFX
  mmLoadEffect(SFX_BELL);
  mmLoadEffect(SFX_KO);
  mmLoadEffect(SFX_CROWD);

  // Setup window on topscreen for super letterboxing

  setBackdropColor(RGB15(0, 3, 5));

  windowSetBounds(WINDOW_1, 1, 43, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 16);

  bgWindowEnable(2, WINDOW_1);
  bgWindowEnable(3, WINDOW_1);
  oamWindowEnable(&oamMain, WINDOW_1);

  bgWindowDisable(2, WINDOW_OUT);
  bgWindowDisable(3, WINDOW_OUT);
  oamWindowDisable(&oamMain, WINDOW_OUT);

  windowDisable(WINDOW_1);

  setBackdropColorSub(RGB15(0, 3, 5));

  // If the music failed to load, go back to the main menu.
  // (If you're reading this and this ever happens, shoot an issue)

  if (mmLoad(matchMusic))
  {
    unloadBoxingRing();
    startMainMenu(false);
  }

  // Congrats, you played once!
  game_data.playedOnce = true;

  return 0;
}

void Game::inInterval(bool show)
{
  interval = show;

  // Load or unload the interval scene

  if (!show)
  {
    NF_DeleteAffineBg(0, 2);
    NF_DeleteAffineBg(0, 3);
    NF_DeleteAffineBg(1, 2);

    NF_UnloadAffineBg("Boxer");
    NF_UnloadAffineBg("TimeoutBg");
    NF_UnloadAffineBg("Bottom");

    NF_LoadAffineBg((currentGame != STORY_GAME)
                        ? stageinformation[customValues[3]][0]
                        : stageinformation[storyCounter - (titleDefense ? 5 : 0)][0],
                    "Arena", 512, 512);
    NF_LoadAffineBg((currentGame != STORY_GAME)
                        ? stageinformation[customValues[3]][1]
                        : stageinformation[storyCounter - (titleDefense ? 5 : 0)][1],
                    "Background", 512, 512);
    NF_LoadAffineBg((currentGame != STORY_GAME)
                        ? stageinformation[customValues[3]][2]
                        : stageinformation[storyCounter - (titleDefense ? 5 : 0)][2],
                    "Bottom", 256, 256);

    NF_CreateAffineBg(1, 2, "Bottom", 0);
    NF_CreateAffineBg(0, 2, "Arena", 0);
    NF_CreateAffineBg(0, 3, "Background", 0);

    NF_FreeSpriteGfx(1, 8);
    NF_UnloadSpriteGfx(8);
    NF_UnloadSpritePal(8);

    NF_DeleteSprite(1, 29);

    setBackdropColor(RGB15(0, 3, 5));

    StopDialogue();
  }
  else
  {
    NF_DeleteAffineBg(0, 2);
    NF_DeleteAffineBg(0, 3);
    NF_DeleteAffineBg(1, 2);

    NF_UnloadAffineBg("Arena");
    NF_UnloadAffineBg("Background");
    NF_UnloadAffineBg("Bottom");

    NF_LoadAffineBg(timeoutBoxer[boxer1.m_character + (titleDefense ? 6 : 0)], "Boxer", 256, 256);
    NF_LoadAffineBg((currentGame != STORY_GAME)
                        ? stageinformation[customValues[3]][3]
                        : stageinformation[storyCounter - (titleDefense ? 5 : 0)][3],
                    "TimeoutBg", 256, 256);
    NF_LoadAffineBg("bg/TimeoutSub", "Bottom", 256, 256);

    NF_CreateAffineBg(1, 2, "Bottom", true);
    NF_CreateAffineBg(0, 2, "Boxer", true);
    NF_CreateAffineBg(0, 3, "TimeoutBg", true);

    NF_LoadSpriteGfx("sprite/cornermen", 8, 64, 64);
    NF_LoadSpritePal("sprite/cornermen", 8);
    NF_VramSpriteGfx(1, 8, 8, true);
    NF_VramSpritePal(1, 8, 8);

    NF_CreateSprite(1, 29, 8, 8, 8, 8);

    StartDialogue();
  }

  // Hide the boxers and referee
  boxer1.hide(!show);
  boxer2.hide(!show);
  ref.hide(!show);
}

int Game::loadWinScreen(bool undisputed)
{
  // Change based on if you come from title defense
  currentScene = undisputed ? UNDISPUTED : WINNER;

  matchMusic = undisputed ? MOD_UNDISPUTEDSCREEN : MOD_CHAMPSCREEN;
  mmLoad(matchMusic);
  mmStart(matchMusic, MM_PLAY_LOOP);

  if (undisputed)
    NF_LoadAffineBg("bg/AChampIsYou", "Winner", 256, 256);
  else
    NF_LoadAffineBg("bg/AWinnerIsYou", "Winner", 256, 256);

  NF_CreateAffineBg(0, 2, "Winner", 0);
  NF_LoadAffineBg("bg/PreviewBg0", "WinnerBg", 256, 256);
  NF_CreateAffineBg(0, 3, "WinnerBg", 1);
  NF_CreateAffineBg(1, 2, "WinnerBg", 1);

  // Cancel crowd sounds
  mmEffectCancel(crowd);
  return 0;
}

void Game::unloadWinScreen()
{

  // Stop and free music and assets
  mmStop();
  mmUnload(matchMusic);

  NF_DeleteAffineBg(0, 2);
  NF_DeleteAffineBg(0, 3);
  NF_DeleteAffineBg(1, 2);
  NF_UnloadAffineBg("Winner");
  NF_UnloadAffineBg("WinnerBg");

  // windowDisableSub(WINDOW_0);
}

int Game::loadPreviewStory()
{

  // If you come from title defense, the assets should differ
  titleDefense = storyCounter > 5;

  if (titleDefense)
    game_data.TDProgress = storyCounter;
  else
    game_data.storyProgress = storyCounter;

  // Save
  if (save_data(save_path))
  {
    nocashMessage("Saved data!\n");
  }
  else
  {
    nocashMessage("Can't save data!\n");
  }

  // Set scene and match
  currentScene = PREVIEW;
  currentMatch = GetMatch(storyCounter);

  // Load all assets and music
  NF_LoadAffineBg(currentMatch->preview, "Preview", 256, 256);
  NF_CreateAffineBg(0, 2, "Preview", 0);
  NF_LoadAffineBg(currentMatch->previewbg, "PreviewBg", 256, 256);
  NF_CreateAffineBg(0, 3, "PreviewBg", 1);
  NF_CreateAffineBg(1, 2, "PreviewBg", 1);

  matchMusic = currentMatch->storyMusic;
  mmLoad(matchMusic);
  mmSetModuleTempo(768);
  mmSetModulePitch(768);
  mmStart(matchMusic, MM_PLAY_LOOP);
  mmSetPosition(1);
  mmEffectCancel(crowd);

  // Start dialogue sequence
  StartDialogue();

  // Set bottom screen letterboxing
  setBackdropColorSub(RGB15(0, 3, 5));
  windowSetBoundsSub(WINDOW_0, 1, 50, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 50);

  for (int i = 0; i < 256; i++)
  {
    bgWindowEnable(i, WINDOW_0);
    bgWindowEnable(i, WINDOW_OUT);
  }

  bgWindowDisable(22, WINDOW_0);
  bgWindowDisable(23, WINDOW_0);

  oamWindowEnable(&oamSub, WINDOW_0);

  windowEnableSub(WINDOW_0);
  return 0;
}

void Game::unloadPreviewStory()
{
  // Stop and free music and assets
  mmUnload(matchMusic);

  NF_DeleteAffineBg(0, 2);
  NF_DeleteAffineBg(0, 3);
  NF_DeleteAffineBg(1, 2);
  NF_UnloadAffineBg("Preview");
  NF_UnloadAffineBg("PreviewBg");

  windowDisableSub(WINDOW_0);

  StopDialogue();
}

void Game::unloadBoxingRing()
{

  // Free all assets, music, and reset all game variables

  boxersLoaded = false;
  boxer1.unload();
  boxer2.unload();
  ref.unload();

  boxer1 = {};
  boxer2 = {};
  ref = {};

  NF_DeleteAffineBg(0, 2);
  NF_DeleteAffineBg(0, 3);
  NF_DeleteAffineBg(1, 2);

  NF_UnloadAffineBg(!interval ? "Arena" : "Boxer");
  NF_UnloadAffineBg(!interval ? "Background" : "TimeoutBg");
  NF_UnloadAffineBg("Bottom");

  StopDialogue();

  windowDisable(WINDOW_1);
  windowDisableSub(WINDOW_0);

  mmStop();
  mmUnload(matchMusic);

  mmEffectCancelAll();

  mmUnloadEffect(SFX_BELL);
  mmUnloadEffect(SFX_KO);
  mmUnloadEffect(SFX_CROWD);

  if (!interval)
  {
    unloadFightTop();
    unloadFightBottom();
  }

  interval = false;
  currentMatch = nullptr;
  customValues = nullptr;

  NF_VramSpriteGfxDefrag(1);
  NF_VramSpriteGfxDefrag(0);
}

void Game::loadFightBottom()
{

  // Load all sprites for the bottom screen
  // (NOTE: set variable names rather than magic numbers for asset loading)

  NF_LoadSpritePal("sprite/bottomfight", 10);

  NF_LoadSpriteGfx("sprite/boxerbars", 10, 64, 64);
  NF_VramSpriteGfx(1, 10, 10, false);

  NF_LoadSpriteGfx("sprite/taphelp", 12, 32, 32);
  NF_VramSpriteGfx(1, 12, 12, true);

  NF_LoadSpriteGfx("sprite/bars", 13, 64, 32);
  NF_VramSpriteGfx(1, 13, 13, true);

  if (titleDefense)
    NF_LoadSpriteGfx("sprite/champportraits", 14, 64, 64);
  else
    NF_LoadSpriteGfx("sprite/portraits", 14, 64, 64);

  NF_VramSpriteGfx(1, 14, 14, false);

  // Loads the bottom sprite pallete into the vram twice, the second time for the yellow bars.
  // This is stupid, don't do this and just create another arbitrary color in your pallete file
  NF_VramSpritePal(1, 10, 10);
  NF_VramSpritePal(1, 10, 11);

  // Sets the color for the stamina bar
  NF_SpriteSetPalColor(1, 11, 1, 31, 22, 13);

  // Create health bars
  for (int i = 0; i < 4; i++)
  {
    int offset = 25;
    int x;
    int y;

    switch (i)
    {
      case 0:
        x = 24;
        y = -20;
        break;
      case 1:
        x = 24;
        y = -4;
        break;
      case 2:
        x = 103;
        y = -20;
        break;
      case 3:
        x = 103;
        y = -4;
        break;
    }
    NF_CreateSprite(1, offset + i, 13, i % 2 == 0 ? 10 : 11, x, y);
    NF_EnableSpriteRotScale(1, offset + i, 25, true);
    NF_SpriteRotScale(1, offset + i, 0, i < 2 ? 280 : -280, 20);
  }

  // Create blank UI (boxerbars)
  for (int i = 0; i < 4; i++)
  {
    int offset = 10;
    NF_CreateSprite(1, offset + i, 10, 10, 0 + (64 * i), 0);
    NF_SpriteFrame(1, offset + i, i > 1 ? (3 - i) : i);
    NF_HflipSprite(1, offset + i, i > 1);
  }

  // Create the portaits for each boxer
  for (int i = 0; i < 2; i++)
  {
    int offset = 23;

    int which = ((currentGame == MP_GAME && !isHost) || titleDefense)
                    ? (i == 0 ? boxer2.m_character : boxer1.m_character)
                    : (i == 0 ? boxer1.m_character : boxer2.m_character);

    NF_CreateSprite(1, offset + i, 14, 10, 0, 0);
    NF_EnableSpriteRotScale(1, offset + i, offset + i, true);
    NF_SpriteRotScale(1, offset + i, 0, 256 * 0.63 * (i == 0 ? -1 : 1), 256 * 0.63);
    NF_MoveSprite(1, offset + i, -38 + (i * 202), -38);
    NF_SpriteFrame(1, offset + i, which);
  }

  // Create the DS icon in the middle of the screen (shown when knocked down)
  NF_CreateSprite(1, 22, 12, 10, (SCREEN_WIDTH / 2) - 16, (SCREEN_HEIGHT / 2) - 16 - 10);

  // r g b are set to the stage color and then the palatte for the boxer bars is changed.

  u8 r = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                : storyCounter - (titleDefense ? 5 : 0)][0];
  u8 g = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                : storyCounter - (titleDefense ? 5 : 0)][1];
  u8 b = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                : storyCounter - (titleDefense ? 5 : 0)][2];

  NF_SpriteSetPalColor(1, 10, 3, r, g, b);
}

void Game::unloadFightBottom()
{

  // Delete, unload, and free all sprites and their assets

  for (int sprites : {10, 12, 13, 14})
  {
    NF_UnloadSpriteGfx(sprites);
    NF_FreeSpriteGfx(1, sprites);
  }

  for (int sprites : {10})
  {
    NF_UnloadSpritePal(sprites);
  }

  for (int sprites : {22, 23, 24, 25, 26, 27, 28})
  {
    NF_DeleteSprite(1, sprites);
  }

  for (int i = 0; i < 4; i++)
  {
    NF_DeleteSprite(1, 10 + i);
  }
}

void Game::loadFightTop()
{

  // Load all sprites for the top screen

  NF_LoadSpriteGfx("sprite/bell", 9, 32, 32);
  NF_LoadSpritePal("sprite/bell", 9);

  NF_VramSpriteGfx(0, 9, 9, true);
  NF_VramSpritePal(0, 9, 9);

  NF_LoadSpriteGfx("sprite/mp_icons", 37, 32, 32);
  NF_LoadSpritePal("sprite/mp_icons", 14);

  NF_VramSpriteGfx(0, 37, 37, false);
  NF_VramSpritePal(0, 14, 14);

  NF_LoadSpriteGfx("sprite/digits", 30, 32, 32);
  NF_LoadSpritePal("sprite/digits", 11);

  NF_LoadSpriteGfx("sprite/downnum", 40, 32, 32);
  NF_LoadSpritePal("sprite/downnum", 13);

  NF_VramSpriteGfx(0, 40, 40, false);
  NF_VramSpritePal(0, 13, 13);

  NF_VramSpriteGfx(0, 30, 30, false);

  // Loads the top pallete for timer digits into the vram twice, the second time for
  // the green numbers.
  // This is stupid, don't do this and just create another arbitrary color in your pallete
  // file
  NF_VramSpritePal(0, 11, 11);
  NF_VramSpritePal(0, 11, 12);

  // Set each pallete for red and green numbers
  NF_SpriteSetPalColor(0, 11, 1, 0, 29, 0);
  NF_SpriteSetPalColor(0, 12, 1, 29, 0, 0);

  // Create the bell
  NF_CreateSprite(0, 30, 9, 9, 128 - 16, 5);

  // Create the numbers for the timer and round counter
  for (int i = 0; i < 4; i++)
  {
    int offset = 31;

    NF_CreateSprite(0, offset + i, 30, i == 0 ? 11 : 12, 0, 0);
    NF_EnableSpriteRotScale(0, offset + i, 10, false);
  }

  // Create the knockdown number
  NF_CreateSprite(0, 35, 40, 13, 128 - 16, 96 - 16);

  // Create the multiplayer identifiers regardless of gametype
  NF_CreateSprite(0, 36, 37, 14, 0, 0);
  NF_CreateSprite(0, 37, 37, 14, 128, 0);
  NF_SpriteFrame(0, 36, 0);
  NF_SpriteFrame(0, 37, 1);

  if (currentGame == MP_GAME)
  {
    NF_EnableSpriteRotScale(0, 36, 11, false);
    NF_EnableSpriteRotScale(0, 37, 11, false);
  }
  else
  {
    NF_ShowSprite(0, 36, false);
    NF_ShowSprite(0, 37, false);
  }

  // Hide the DS icon
  NF_ShowSprite(0, 35, false);
}

void Game::unloadFightTop()
{
  // Delete, unload, and free all assets

  for (int gfx : {9, 30, 40, 37})
  {
    NF_UnloadSpriteGfx(gfx);
    NF_FreeSpriteGfx(0, gfx);
  }

  for (int pal : {9, 11, 13, 14})
  {
    NF_UnloadSpritePal(pal);
  }

  for (int sprites : {30, 31, 32, 33, 34, 35, 36, 37})
  {
    NF_DeleteSprite(0, sprites);
  }
}

void Game::startIntro()
{

  // Load scene sounds
  mmLoadEffect(SFX_INTRO_ITALY);
  mmLoadEffect(SFX_INTRO_PAPER);

  mmLoadEffect(SFX_LIGHT);
  mmLoadEffect(SFX_KO);
  mmLoadEffect(SFX_BELL);
  mmLoadEffect(SFX_CROWD);

  // Set the screen to black
  setBrightness(3, -16);

  // Set scene to intro
  currentScene = INTROCUTSCENE;

  // Load the first intro bg

  NF_LoadAffineBg("bg/Intro", "intro", 256, 256);
  NF_CreateAffineBg(0, 2, "intro", false);

  // Create the center window for the top screen

  setBackdropColor(RGB15(0, 3, 5));

  windowSetBounds(WINDOW_1, 48, 32, 208, 152);

  bgWindowEnable(2, WINDOW_1);
  bgWindowEnable(3, WINDOW_1);
  oamWindowEnable(&oamMain, WINDOW_1);

  bgWindowDisable(2, WINDOW_OUT);
  bgWindowDisable(3, WINDOW_OUT);
  oamWindowDisable(&oamMain, WINDOW_OUT);

  windowEnable(WINDOW_1);
}

void Game::unloadIntro()
{
  // Delete and unload all the assets for the intro scene.

  NF_DeleteAffineBg(0, 2);
  NF_UnloadAffineBg("intro");

  mmEffectCancelAll();
  mmUnloadEffect(SFX_INTRO_ITALY);
  mmUnloadEffect(SFX_INTRO_PAPER);

  mmUnloadEffect(SFX_LIGHT);
  mmUnloadEffect(SFX_KO);
  mmUnloadEffect(SFX_BELL);
  mmUnloadEffect(SFX_CROWD);

  windowDisable(WINDOW_1);
}

void Game::startMainMenu(bool intro)
{

  // Disable the window if it's being used
  windowDisable(WINDOW_1);

  // Save
  if (save_data(save_path))
  {
    nocashMessage("Saved data!\n");
  }
  else
  {
    nocashMessage("Can't save data!\n");
  }

  // Set scene to menu
  currentScene = MENU;

  Transition(3, intro ? 16 : -16, 0, false);

  // Call load on the menu for this game class.
  menu.Load(showCredits ? WONTD : intro ? TITLE : MAIN);

  // This is true if you beat title defense for the first time.
  // Set it to false.
  showCredits = false;
}

// The input handler for this game
void Game::handleEvents()
{
  scanKeys();

  if (!lockInput) // Only do everything if you're allowed to give inputs
  {

    // Handles the input if you are in the main menu.
    if (currentScene == MENU)
    {
      switch (menu.handleInput())
      {
        case START_NEW_GAME:
          Transition(3, -16, 0, true);
          menu.Unload();
          game_data.storyProgress = 1;
          storyCounter = 1;

          for (int i = 0; i < 10; i++)
          {
            game_data.storyTimes[i] = i < 5 ? 3 : 10800;
          }

          loadPreviewStory();
          break;
        case START_PREV_GAME:
          Transition(3, -16, 0, true);
          menu.Unload();
          storyCounter = game_data.storyProgress;
          loadPreviewStory();
          break;

        case START_NEW_TD:
          Transition(3, -16, 0, true);
          menu.Unload();
          game_data.TDProgress = 6;
          storyCounter = 6;

          for (int i = 0; i < 10; i++)
          {
            game_data.storyTimes[i] = i < 5 ? 3 : 10800;
          }

          loadPreviewStory();
          break;

        case START_PREV_TD:
          Transition(3, -16, 0, true);
          menu.Unload();
          storyCounter = game_data.TDProgress;
          loadPreviewStory();
          break;

        case START_FREEPLAY:
          Transition(3, -16, 0, true);
          windowDisable(WINDOW_0);
          menu.Unload();
          loadBoxingRing(FREE_GAME, 0);
          break;

        case START_MULTIPLAYER:
          Transition(3, -16, 0, true);
          windowDisable(WINDOW_0);
          menu.Unload();
          loadBoxingRing(MP_GAME, 0);
          break;
        default:
          break;
      }
    }

    // Story mode preview screen, if you press A you fight
    // If you press B, you go back.

    if (currentScene == PREVIEW && brightness == 0)
    {
      if (keysDown() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
      {
        Transition(3, -16, 0, true);
        unloadPreviewStory();
        loadBoxingRing(STORY_GAME, storyCounter);
      }

      if (keysDown() & KEY_B)
      {
        unloadPreviewStory();
        startMainMenu(false);
      }
    }

    // Skip Intro
    if (currentScene == INTROCUTSCENE)
    {
      if (keysDown() & KEY_A || keysDown() & KEY_START)
      {
        unloadIntro();
        startMainMenu(true);
      }
    }

    // Exit results screen
    if ((currentScene == WINNER || currentScene == UNDISPUTED) && brightness == 0)
    {
      if (keysDown() & KEY_A)
      {
        Transition(3, -16, 0, true);
        unloadWinScreen();
        startMainMenu(false);
      }
    }

    // Handle all inputs during the fight.
    if (currentScene == GAME)
    {
      if (boxersLoaded && (!paused))
      {
        if (roundActive)
          // Handle the input for the boxer you control.
          boxer1.handleInput();
      }

      // If you are in the interval scene, leave
      if (keysDown() & KEY_A && (!lockInput) && interval)
      {
        intervalTransition(false, false);
      }

      // Pause
      if (keysDown() & KEY_START && roundActive)
      {
        paused = !paused;
        if (paused)
        {
          setBrightness(3, -8);
          mmSetModuleVolume(game_data.muteMusic ? 0 : 1024 / 8);
        }
        else
        {
          setBrightness(3, playerDowned ? -8 : 0);
          mmSetModuleVolume(game_data.muteMusic ? 0 : 1024 / 3);
        }
      }

      // Set paused to false if the timer reaches zero just to prevent any issues.
      if (!roundActive)
      {
        paused = false;
      }

      // Leave the game and return to the main menu.
      if (keysDown() & KEY_B && paused)
      {
        paused = false;
        disable_multiplayer();
        Transition(3, -16, 0, true);
        unloadBoxingRing();
        startMainMenu(false);
      }
    }
  }
}

// Update the sprites and visuals of the bottom screen during a fight.
void Game::updateFightBottom()
{
  if (currentScene == GAME)
  {
    int barStats[6] = {int(lerp(0, -71, (150.0f - boxer1.m_hp) / 150.f)),
                       int((-0.71f * (100 - boxer1.m_stamina)) - ((8.0f * boxer1.m_stamina) / 100)),
                       int(lerp(0, 71, (150.0f - boxer2.m_hp) / 150.f)),
                       int((0.71f * (100 - boxer2.m_stamina)) + ((8.0f * boxer2.m_stamina) / 100)),

                       int(floor(boxer1.m_super) / 50) + 1,
                       int(floor(boxer2.m_super) / 50) + 1};

    int mpbarStats[6] = {
        int(lerp(0, -71, (150.0f - boxer2.m_hp) / 150.f)),
        int((-0.71f * (100 - boxer2.m_stamina)) - ((8.0f * boxer2.m_stamina) / 100)),
        int(lerp(0, 71, (150.0f - boxer1.m_hp) / 150.f)),
        int((0.71f * (100 - boxer1.m_stamina)) + ((8.0f * boxer1.m_stamina) / 100)),

        int(floor(boxer2.m_super) / 50) + 1,
        int(floor(boxer1.m_super) / 50) + 1};

    bool switchbars = ((currentGame == MP_GAME) && (!isHost)) || titleDefense;

    for (int i = 0; i < 6; i++)
    {
      if (i < 4)
      {
        int offset = 25;
        int x;
        int y;

        switch (i)
        {
          case 0:
            x = 24;
            y = -20;
            break;
          case 1:
            x = 24;
            y = -4;
            break;
          case 2:
            x = 103;
            y = -20;
            break;
          case 3:
            x = 103;
            y = -4;
            break;
        }
        NF_MoveSprite(1, offset + i, x + (switchbars ? mpbarStats[i] : barStats[i]), y);
      }
      else
        NF_SpriteFrame(
            1, 10 + i - 3,
            std::min((switchbars ? mpbarStats[i] : barStats[i]), 6) +
                ((switchbars ? mpbarStats[i] : barStats[i]) == 6 && (tick % 30 > 15) ? 1 : 0));
    }

    if (boxer1.m_state != DOWN || !matchActive)
    {
      mmSetModulePitch(titleDefense ? 960 : 1024);
      u8 r = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                    : storyCounter - (titleDefense ? 5 : 0)][0];
      u8 g = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                    : storyCounter - (titleDefense ? 5 : 0)][1];
      u8 b = stagecolor[(currentGame != STORY_GAME) ? customValues[3]
                                                    : storyCounter - (titleDefense ? 5 : 0)][2];

      NF_SpriteSetPalColor(1, 10, 3, r, g, b);

      NF_ShowSprite(1, 22, false);
      NF_ShowBg(1, 2);
    }
    else if ((boxer1.m_state == DOWN && boxer1.m_rounddowns != 3))
    {
      float spam = boxer1.m_getUpCount / 100.0f;

      mmSetModulePitch((1024 / 2) + (rand() % 200) - 100);

      float r = lerp(31, 0, spam);
      float g = lerp(31, 31, spam);
      float b = lerp(31, 0, spam);

      NF_DefineTextColor(1, 0, 10, r, g, b);
      NF_SpriteSetPalColor(1, 10, 3, 0, 3, 5);

      setBrightness(1, lerp(-14, 0, spam));

      NF_ShowSprite(1, 22, true);
      NF_SpriteFrame(1, 22, (tick / 12) % 3);
      NF_SetTextColor(1, 0, 10);
      NF_WriteText(1, 0, 9, 13, "Tap To Get Up!");
      NF_SetTextColor(1, 0, 1);

      NF_HideBg(1, 2);

      if (keysDown() & KEY_TOUCH)
      {
        Transition(2, -4, -8, false);
      }
    }
  }
}

int bellAnim = 0;

void Game::updateFightTop()
{
  char timer[5];

  snprintf(timer, sizeof(timer), ((roundTimer / 60) % 60) < 10 ? "%i0%i" : "%i%i",
           (roundTimer / 60) / 60, (roundTimer / 60) % 60);

  int min = (roundTimer / 60) / 60;
  int sec1 = ((roundTimer / 60) % 60) < 10 ? 0 : (roundTimer / 60) % 60 / 10;
  int sec2 = ((roundTimer / 60) % 60) % 10;

  for (int i = 0; i < 4; i++)
  {
    int offset = 31;
    int x;
    int y;

    switch (i)
    {
      case 0:
        x = 87 - 12;
        y = 24;
        break;
      case 1:
        x = 130 - 12;
        y = 24;
        break;
      case 2:
        x = 160 - 12;
        y = 24;
        break;
      case 3:
        x = 176 - 12;
        y = 24;
        break;
    }

    switch (i)
    {
      case 0:
        NF_SpriteFrame(0, offset + i, std::clamp(roundNumber, 0, 15));
        break;
      case 1:
        NF_SpriteFrame(0, offset + i, std::clamp(min, 0, 15));
        break;
      case 2:
        NF_SpriteFrame(0, offset + i, std::clamp(sec1, 0, 15));
        break;
      case 3:
        NF_SpriteFrame(0, offset + i, std::clamp(sec2, 0, 15));
        break;
    }

    int newx;
    int newy;

    worldToScreen(x, y, newx, newy, 32);

    NF_SpriteRotScale(0, 10, 0, int(256 * fScale), int(256 * fScale));
    NF_MoveSprite(0, offset + i, newx, newy);
  }

  for (int i : {36, 37})
  {
    int leftsidex = (isHost ? boxer1.m_x : boxer2.m_x) + 4;
    int rightsidex = (isHost ? boxer2.m_x : boxer1.m_x) + 40;

    int x = (i == 36 ? leftsidex : rightsidex);
    int y = 80;

    int newx;
    int newy;

    worldToScreen(x, y, newx, newy, 16);

    NF_SpriteRotScale(0, 11, 0, int(128 * fScale), int(128 * fScale));
    NF_MoveSprite(0, i, newx, newy);
  }

  NF_ShowSprite(
      0, 30,
      (((tick % 2 == 0 && abs(tick - whenToStart) < 60) || abs(tick - whenToStart) < 30) ||
       ((tick % 2 == 0 && roundTimer < 60) || roundTimer < 30)) &&
          (makingDecision == 0 && matchActive)

  );

  NF_SpriteFrame(0, 30, std::clamp(((tick - whenToStart) / 6), 0, 5));
  if (roundActive == false && roundTimer == 0)
  {
    NF_SpriteFrame(0, 30, std::clamp(((bellAnim) / 6), 0, 5));
    bellAnim++;
  }

  if (boxer1.m_state == SUPERSEQ || boxer2.m_state == SUPERSEQ)
  {
    sequenceSuper = std::clamp(sequenceSuper + 1, 0, 60);

    if (sequenceSuper == 1)
    {
      windowEnable(WINDOW_1);
    }
  }
  else
  {
    sequenceSuper = std::clamp(sequenceSuper - 1, 0, 60);
    if (sequenceSuper == 0)
    {
      windowDisable(WINDOW_1);
    }
  }

  float lerper = std::clamp(sequenceSuper / 60.0f, 0.0f, 1.0f);
  // lerper = 1;

  int toplimit = (boxer1.m_character == SLIMJIM || boxer2.m_character == SLIMJIM) ? 43 : 65;

  windowSetBounds(WINDOW_1, 1, lerp(0, toplimit, (1 - std::pow(1 - lerper, 2))), SCREEN_WIDTH - 1,
                  SCREEN_HEIGHT - lerp(0, 16, (1 - std::pow(1 - lerper, 2))));
}

void Game::intervalTransition(bool transitioning_in, bool updater)
{
  if (!updater)
  {
    sequenceInterval = -5;
    transitioningIntoInterval = transitioning_in;
    return;
  }

  sequenceInterval++;

  if (transitioningIntoInterval)
  {
    switch (sequenceInterval)
    {
      case 0:
        break;
      case 120:
        Transition(3, 0, -16, true);
        break;
      case 160:
        Transition(3, -16, 0, true);
        inInterval(true);

        unloadFightTop();
        unloadFightBottom();

        mmUnload(matchMusic);

        mmLoad(MOD_INTERMISSION);

        mmStart(MOD_INTERMISSION, MM_PLAY_LOOP);

        break;
      default:
        break;
    }
  }

  if (!transitioningIntoInterval)
  {
    switch (sequenceInterval)
    {
      case 0:
        Transition(3, 0, -16, true);
        mmStop();
        if (roundNumber == maxRounds)
        {
          decision(false);
        }
        else
        {
          newRound(160, false);
        }
        break;

      case 40:
        Transition(3, -16, 0, true);
        loadFightTop();
        loadFightBottom();

        inInterval(false);
        mmUnload(MOD_INTERMISSION);
        mmLoad(matchMusic);

        break;
      default:
        break;
    }
  }
}

int Game::manageFight()
{
  if (boxerDown)
  {
    expectedWinner = boxer1.m_state == DOWN ? 2 : 1;
  }
  else if ((boxer1.m_downs + boxer2.m_downs) > 0)
  {
    expectedWinner = boxer1.m_downs >= boxer2.m_downs ? 2 : 1;
    if (((currentGame == MP_GAME) && isHost) && boxer2.m_downs == boxer1.m_downs)
      expectedWinner = 1;
  }
  else
  {
    expectedWinner = boxer2.m_hp >= boxer1.m_hp ? 2 : 1;
    if (((currentGame == MP_GAME) && isHost) && boxer2.m_hp == boxer1.m_hp)
      expectedWinner = 1;
  }

  if (roundTimer == 0 && boxer1.m_state != DOWN && boxer2.m_state != DOWN && roundActive)
  {
    bellAnim = 0;
    roundActive = false;
    intervalTransition(true, false);
    mmEffect(SFX_BELL);
    mmStop();
    setBrightness(3, 0);
  }

  if (!matchActive && roundNumber > 0)
  {

    int currenttime = roundNumber * (10800 - roundTimer);

    int recordtime;
    bool newRecord;

    if (titleDefense)
    {
      recordtime = game_data.recordTD[storyCounter - 6] * game_data.recordTD[storyCounter - 1];
      newRecord = currenttime < recordtime || game_data.recordTD[storyCounter - 6] == -1;
    }
    else
    {
      recordtime =
          game_data.recordTimes[storyCounter - 1] * game_data.recordTimes[storyCounter + 4];
      newRecord = currenttime < recordtime || game_data.recordTimes[storyCounter - 1] == -1;
    }

    if ((tick - sequenceMatchOver) >= 60)
    {

      int time = 10800 - roundTimer;

      char boxerInfo[50];

      int min = (time / 60) / 60;
      int sec1 = ((time / 60) % 60) < 10 ? 0 : (time / 60) % 60 / 10;
      int sec2 = ((time / 60) % 60) % 10;

      sprintf(boxerInfo, "R%i %i:%i%i", roundNumber, min, sec1, sec2);

      NF_WriteText(1, 0, 8, 10,
                   boxer1.m_state == DOWN     ? "   You Lose...   "
                   : boxer2.m_rounddowns == 3 ? "      TKO!!      "
                   : boxerDown                ? "    Knockout!    "
                                              : "Majority Decision");

      NF_WriteText(1, 0, 13, 12, expectedWinner == 1 ? boxerInfo : "");
      NF_SetTextColor(1, 0, tick % 8 > 4 ? 2 : 1);
      NF_WriteText(1, 0, 11, 14,
                   (currentGame == STORY_GAME) && (newRecord && expectedWinner == 1) ? "New Record!"
                                                                                     : "");
      NF_SetTextColor(1, 0, 1);
    }

    switch (tick - sequenceMatchOver)
    {
      case (30):
        mmEffect(SFX_BELL);
        mmStop();
        break;
      case (40):
        mmEffect(SFX_BELL);
        break;
      case (50):
        mmEffect(SFX_BELL);
        break;
      case (60):
        NF_ShowSprite(0, 35, false);

        if (ref.m_state != DECISION)
          mmEffect(SFX_REF_ITSOVER);
        Transition(3, 8, 0, false);

        break;

      case (100):
        if (expectedWinner == 1)
        {
          mmEffect(boxer1.sfxwin);
        }
        else
        {
          mmEffect(boxer2.sfxwin);
        }

        break;

      case (400):
        Transition(3, 0, -16, true);
        break;
      case (440):
        Transition(3, -16, 0, true);

        if (expectedWinner == 1 && (currentGame == STORY_GAME))
        {
          unloadBoxingRing();
          if (storyCounter == 5 || storyCounter == 10)
          {
            Transition(3, 16, 0, true);

            if (titleDefense)
            {
              game_data.TDTimes[storyCounter - 6] = roundNumber;
              game_data.TDTimes[storyCounter - 1] = 10800 - roundTimer;
            }
            else
            {
              game_data.storyTimes[storyCounter - 1] = roundNumber;
              game_data.storyTimes[storyCounter + 4] = 10800 - roundTimer;
            }

            if (newRecord)
            {

              if (titleDefense)
              {
                game_data.recordTD[storyCounter - 6] = roundNumber;
                game_data.recordTD[storyCounter - 1] = 10800 - roundTimer;
              }
              else
              {
                game_data.recordTimes[storyCounter - 1] = roundNumber;
                game_data.recordTimes[storyCounter + 4] = 10800 - roundTimer;
              }
            }

            bool undisputed = titleDefense;

            storyCounter = 1;
            if (titleDefense)
            {

              if (!game_data.wonTDOnce)
                showCredits = true;

              game_data.TDProgress = 6;
              game_data.wonTDOnce = true;
            }
            else
            {
              game_data.storyProgress = 1;
              game_data.wonOnce = true;
            }

            loadWinScreen(undisputed);
          }
          else
          {
            if (titleDefense)
            {
              game_data.TDTimes[storyCounter - 6] = roundNumber;
              game_data.TDTimes[storyCounter - 1] = 10800 - roundTimer;
            }
            else
            {
              game_data.storyTimes[storyCounter - 1] = roundNumber;
              game_data.storyTimes[storyCounter + 4] = 10800 - roundTimer;
            }

            if (newRecord)
            {

              if (titleDefense)
              {
                game_data.recordTD[storyCounter - 6] = roundNumber;
                game_data.recordTD[storyCounter - 1] = 10800 - roundTimer;
              }
              else
              {
                game_data.recordTimes[storyCounter - 1] = roundNumber;
                game_data.recordTimes[storyCounter + 4] = 10800 - roundTimer;
              }
            }

            storyCounter++;
            if (titleDefense)
              game_data.TDProgress = storyCounter;
            else
              game_data.storyProgress = storyCounter;

            loadPreviewStory();
          }
        }
        else
        {

          if (currentGame == MP_GAME)
          {
            game_data.recordMultiplayer[boxer1.m_state == DOWN ? 1 : 0][boxer1.m_character]++;
            if (boxer2.m_state == DOWN)
              game_data.recordMultiplayer[2][boxer1.m_character]++;
          }

          unloadBoxingRing();
          startMainMenu(false);
          disable_multiplayer();
        }

        return 1;

        break;
    }
  }

  if (currentScene == GAME && roundActive)
  {
    if ((currentGame != MP_GAME) || isHost)
    {
      if ((!paused || (currentGame == MP_GAME)) && roundActive && roundTimer > 0)
        roundTimer -= timerSpeed;
    }

    if (boxer1.m_state == DOWN || boxer2.m_state == DOWN)
    {
      if (!boxerDown)
      {
        mmEffect(SFX_KO);
        if (boxer2.m_rounddowns == 3 || boxer1.m_rounddowns == 3)
        {
          if (currentGame != MP_GAME)
            boxer2.killAi();
          matchActive = false;
          roundActive = false;
          sequenceMatchOver = tick;
        }
      }
      boxerDown = true;
    }
    else
    {
      boxerDown = false;
    }

    if (boxerDown)
    {
      downTimer++;
      if (downTimer % 60 == 0)
      {
        NF_SpriteFrame(0, 35, (downTimer / 60));
        NF_ShowSprite(0, 35, true);
      }
      if (downTimer == 540)
      {
        if (boxer1.m_state == DOWN)
          Transition(3, -8, 0, false);
        if (currentGame != MP_GAME)
          boxer2.killAi();
        matchActive = false;
        roundActive = false;
        sequenceMatchOver = tick;
      }
    }
    else
    {
      downTimer = -45;
      NF_ShowSprite(0, 35, false);
    }

    if (boxer1.m_state != DOWN)
    {
      if (playerDowned)
      {
        Transition(3, -8, 0, false);
        playerDowned = false;
      }
    }
    else
    {
      if (!playerDowned)
      {
        Transition(3, 5, matchActive ? -8 : 0, false);
        playerDowned = true;
      }
    }
  }

  return 0;
}

void Game::crowdSounds()
{
  if (((tick - crowdStart) % 120) == 5)
  {
    mmEffectCancel(crowd);
    crowd = mmEffect(SFX_CROWD);
  }

  if ((!roundActive || boxerDown || (sequenceSuper > 0) || !matchActive) && !interval)
  {
    crowdVolume = std::clamp(crowdVolume + 5, 30, 255);
  }
  else
  {
    crowdVolume = std::clamp(crowdVolume - 1, 30, 255);
  }

  mmEffectVolume(crowd, crowdVolume);
}

void Game::cameraHandler()
{
  if (currentScene == GAME)
  {
    // Define variables for fight centering & game over panning

    float lerper = std::clamp(((tick - sequenceMatchOver) * !matchActive) / 120.0f, 0.0f, 1.0f);
    float regScale =
        std::clamp(std::pow((192.0f / (abs(boxer1.m_x - boxer2.m_x))), 0.3f), 0.75f, 1.5f);

    int winnerPos = lerp(128, (expectedWinner == 1 ? (boxer1.m_x + 32) : (boxer2.m_x + 32)), 0.8);

    fZoomCenterX = lerp(int(((boxer1.m_x + (32)) + (boxer2.m_x + (32))) / 2), winnerPos,
                        1 - std::pow(1 - lerper, 4));

    fZoomCenterY = int(((boxer1.m_y + (64)) + (boxer2.m_y + (64))) / 2);

    // Handle camera zooming

    int beforeZoomX, beforeZoomY;
    screenToWorld(fZoomCenterX, fZoomCenterY, beforeZoomX, beforeZoomY);

    fScale = lerp(regScale, 1.5f, (1 - std::pow(1 - lerper, 2)));

    int afterZoomX, afterZoomY;
    screenToWorld(fZoomCenterX, fZoomCenterY, afterZoomX, afterZoomY);

    fOffsetX = fZoomCenterX - int(128 / fScale);

    fOffsetX += (beforeZoomX - afterZoomX);
    fOffsetY += (beforeZoomY - afterZoomY);

    NF_AffineBgMove(0, 2, fOffsetX + 128, fOffsetY + 128, 0);
    NF_AffineBgTransform(0, 2, int(256 / fScale), int(256 / fScale), 0, 0);
    NF_AffineBgMove(0, 3, fOffsetX + 128, fOffsetY + 128, 0);
    NF_AffineBgTransform(0, 3, int(256 / fScale), int(256 / fScale), 0, 0);
  }

  if (currentScene == PREVIEW || currentScene == WINNER || currentScene == UNDISPUTED)
  {
    NF_AffineBgMove(0, 3, 0, -(tick), 0);
    NF_AffineBgMove(1, 2, 0, (tick), 0);

    NF_AffineBgMove(0, 2, 0, int(3 * sin(tick / 30.0f)) - 3, 0);
  }

  if (interval)
  {
    float lerper = std::clamp(brightness / -16.0f, 0.0f, 1.0f);

    int zoomBy = int(lerp(0.0f, -80.0f, std::pow(lerper, 1.6)));

    NF_AffineBgMove(0, 3, 0 - (zoomBy / 2) + int(3 * sin(tick / 30.0f)), 0 - (zoomBy / 5), 0);
    NF_AffineBgMove(0, 2, 0 - (zoomBy / 2) + int(3 * sin(tick / 30.0f)), 0 - (zoomBy / 5), 0);
    NF_AffineBgMove(1, 2, 0, 0, 0);

    NF_AffineBgTransform(0, 3, 256 + zoomBy, 256 + zoomBy, 0, 0);
    NF_AffineBgTransform(0, 2, 256 + zoomBy, 256 + zoomBy, 0, 0);
    NF_AffineBgTransform(1, 2, 256, 256, 0, 0);
  }
}

void Game::newRound(int when, bool updater)
{
  if (updater)
  {
    if ((whenToStart - tick) < 120 && (currentGame == MP_GAME) && !roundActive && matchActive &&
        !interval && !transitioningIntoInterval && makingDecision == 0)
    {
      setBrightness(3, 0);
    }
    if ((whenToStart - tick) < 0 && !roundActive && matchActive && !interval &&
        !transitioningIntoInterval && makingDecision == 0)
    {
      roundActive = true;
      mmLoadEffect(SFX_BELL);
      mmEffect(SFX_BELL);
      mmStart(matchMusic, MM_PLAY_LOOP);
    }
    return;
  }

  roundNumber++;
  roundTimer = 10800;
  boxer1.resetPosition();
  boxer2.resetPosition();
  ref.resetPosition();
  crowdStart = tick;
  whenToStart = tick + when;
}

void Game::decision(bool updater)
{
  if (updater)
  {
    int whoToPick = expectedWinner;

    if (makingDecision > 0)
      makingDecision++;

    switch (makingDecision)
    {
      case 0:
        break;
      case 30:
        setBrightness(3, 0);
        break;
      case (4 * 60):

        if (((currentGame == MP_GAME) && !isHost) || titleDefense)
        {
          whoToPick = (expectedWinner == 1) ? 2 : 1;
        }
        mmEffect(SFX_REF_WINNER);
        ref.decisionSequence(whoToPick, false);
        break;
      case (5 * 60):
        boxer2.killAi();
        matchActive = false;
        roundActive = false;
        sequenceMatchOver = tick;
        makingDecision = 0;
        break;
    }
    return;
  }

  boxer1.resetPosition();
  boxer2.resetPosition();
  ref.resetPosition();
  crowdStart = tick;

  makingDecision = 1;
  ref.decisionSequence(expectedWinner, true);
}

void Game::update()
{
  NF_ClearTextLayer(1, 0);

  if ((currentGame == MP_GAME) && currentScene == GAME && mp_game.connected == 1)
  {
    tick_multiplayer();
    pkt_client_to_host packet;
    if (isHost)
    {
      mp_game.player[0].ready = (boxersLoaded && !interval);
      mp_game.gametimer = roundTimer;
      mp_game.downtimer = downTimer;

      mp_game.player[0].x = boxer1.m_x;

      mp_game.player[0].m_juked = boxer1.m_juked;
      mp_game.player[0].m_punchFreq = boxer1.m_punchFreq;

      mp_game.player[0].m_hp = boxer1.m_hp;
      mp_game.player[0].m_stamina = boxer1.m_stamina;
      mp_game.player[0].m_block = boxer1.m_block;
      mp_game.player[0].m_super = boxer1.m_super;
      mp_game.player[0].m_state = boxer1.m_state;
      mp_game.player[0].m_downs = boxer1.m_downs;
      mp_game.player[0].m_rounddowns = boxer1.m_rounddowns;

      mp_game.player[0].m_damagedAlready = boxer1.m_damagedAlready;
      mp_game.player[0].m_punchedAlready = boxer1.m_punchedAlready;
      mp_game.player[0].m_getUpCount = boxer1.m_getUpCount;

      mp_game.player[0].m_boxerLocked = boxer1.m_boxerLocked;
      mp_game.player[0].m_intClock = boxer1.m_intClock;
      mp_game.player[0].m_frame = boxer1.m_frame;

      multiplayerReady = mp_game.player[1].ready && mp_game.player[0].ready;

      boxer2.m_x = mp_game.player[1].x;

      boxer2.m_juked = mp_game.player[1].m_juked;
      boxer2.m_punchFreq = mp_game.player[1].m_punchFreq;

      boxer2.m_hp = mp_game.player[1].m_hp;
      boxer2.m_stamina = mp_game.player[1].m_stamina;
      boxer2.m_block = mp_game.player[1].m_block;
      boxer2.m_super = mp_game.player[1].m_super;
      boxer2.m_state = mp_game.player[1].m_state;
      boxer2.m_downs = mp_game.player[1].m_downs;
      boxer2.m_rounddowns = mp_game.player[1].m_rounddowns;

      boxer2.m_damagedAlready = mp_game.player[1].m_damagedAlready;
      boxer2.m_punchedAlready = mp_game.player[1].m_punchedAlready;
      boxer2.m_getUpCount = mp_game.player[1].m_getUpCount;

      boxer2.m_boxerLocked = mp_game.player[1].m_boxerLocked;
      boxer2.m_intClock = mp_game.player[1].m_intClock;
      boxer2.m_frame = mp_game.player[1].m_frame;
    }
    else
    {
      packet.ready = (boxersLoaded && !interval);

      packet.x = boxer1.m_x;

      packet.m_hp = boxer1.m_hp;

      packet.m_juked = boxer1.m_juked;
      packet.m_punchFreq = boxer1.m_punchFreq;

      packet.m_stamina = boxer1.m_stamina;
      packet.m_block = boxer1.m_block;
      packet.m_super = boxer1.m_super;
      packet.m_state = boxer1.m_state;
      packet.m_downs = boxer1.m_downs;
      packet.m_rounddowns = boxer1.m_rounddowns;

      packet.m_damagedAlready = boxer1.m_damagedAlready;
      packet.m_punchedAlready = boxer1.m_punchedAlready;
      packet.m_getUpCount = boxer1.m_getUpCount;

      packet.m_boxerLocked = boxer1.m_boxerLocked;
      packet.m_intClock = boxer1.m_intClock;
      packet.m_frame = boxer1.m_frame;

      Wifi_MultiplayerClientReplyTxFrame(&packet, sizeof(packet));

      multiplayerReady = mp_game.player[1].ready && mp_game.player[0].ready;
      roundTimer = mp_game.gametimer;
      downTimer = mp_game.downtimer;

      boxer2.m_x = mp_game.player[0].x;

      boxer2.m_juked = mp_game.player[0].m_juked;
      boxer2.m_punchFreq = mp_game.player[0].m_punchFreq;

      boxer2.m_hp = mp_game.player[0].m_hp;
      boxer2.m_stamina = mp_game.player[0].m_stamina;
      boxer2.m_block = mp_game.player[0].m_block;
      boxer2.m_super = mp_game.player[0].m_super;
      boxer2.m_state = mp_game.player[0].m_state;
      boxer2.m_downs = mp_game.player[0].m_downs;
      boxer2.m_rounddowns = mp_game.player[0].m_rounddowns;

      boxer2.m_damagedAlready = mp_game.player[0].m_damagedAlready;
      boxer2.m_punchedAlready = mp_game.player[0].m_punchedAlready;
      boxer2.m_getUpCount = mp_game.player[0].m_getUpCount;

      boxer2.m_boxerLocked = mp_game.player[0].m_boxerLocked;
      boxer2.m_intClock = mp_game.player[0].m_intClock;
      boxer2.m_frame = std::max(mp_game.player[0].m_frame, 0);
    }

    if (get_multiplayer_status() == MP_CONNECTION_LOST)
    {
      Transition(3, -16, 0, true);
      unloadBoxingRing();
      startMainMenu(false);
      disable_multiplayer();
    }
  }

  if (!paused || (currentGame == MP_GAME))
  {
    tick++;
    TransitionUpdate(tick);
    cameraHandler();

    if ((currentGame == MP_GAME) && paused)
    {
      {
        NF_ClearTextLayer(1, 0);

        NF_WriteText(1, 0, 9, 11, "Start: Continue");
        NF_WriteText(1, 0, 9, 12, "B: Forfeit");
      }
    }

    if (boxersLoaded && !interval)
    {
      boxer1.update(boxer2, roundActive && ref.allowMovement, matchActive);
      boxer2.update(boxer1, roundActive && ref.allowMovement, matchActive);
      ref.update(boxer1, boxer2, roundActive, matchActive);
    }

    if (currentScene == MENU)
    {
      menu.Update();
      if (tick % 60 == 0)
      {
        save_data(save_path);
      }
    }

    if (currentScene == GAME && boxersLoaded)
    {
      if ((currentGame != MP_GAME) || ((whenToStart - tick < 60) && !ref.makingDecision) ||
          multiplayerReady)
      {
        if (!manageFight() && !interval)
        {
          updateFightBottom();
          updateFightTop();
        }
        decision(true);
        newRound(0, true);
      }
      else
      {
        whenToStart++;

        if (ref.makingDecision)
        {
          makingDecision = 1;
          ref.m_intClock = 0;
        }

        if (!interval)
        {
          NF_WriteText(1, 0, 5, 12, "Waiting For Opponent...");
          setBrightness(1, -16);
        }
      }
      intervalTransition(0, true);
      crowdSounds();
    }

    if (currentScene == GAME && interval)
    {
      NF_SetTextColor(1, 0, 1);
      NF_WriteText(1, 0, 20, 22, btnfonts[FNT_A]);
      NF_WriteText(1, 0, 21, 22, ": Continue");
      NF_SetTextColor(1, 0, 3);

      char helpfultext[50];
      for (int i = 0; i < 3; i++)
      {
        switch (i)
        {
          case 0:
            snprintf(helpfultext, 15, "Mss.| %i", boxer1.getStatistics(0)

            );
            break;
          case 1:
            snprintf(helpfultext, 15, "Hit.| %i", boxer1.getStatistics(1)

            );
            break;
          case 2:
            snprintf(helpfultext, 15, "Acc.| %i%%", boxer1.getStatistics(2)

            );
            break;
        }

        NF_WriteText(1, 0, 19, 7 + (2 * i), helpfultext);
      }

      CornermenDialogue dynamicTalk;
      dynamicTalk = (roundNumber == maxRounds) ? REFWILLDECIDE
                    : (expectedWinner != 1)    ? GETTING_HURT
                    : boxer1.m_tireouts > 3    ? GETTING_TIRED
                                               : DOING_WELL;

      NF_SetTextColor(1, 0, 1);
      if (brightness == 0 || transitioningIntoInterval)
        RunDialogue(cornermenDialogue[boxer1.m_character][dynamicTalk], 5, 3, 2, 10);

      NF_SpriteFrame(1, 29, ((int)boxer1.m_character * 4) + ((tick / 12) % 4));
    }

    if (currentScene == PREVIEW)
    {
      RunDialogue(currentMatch->quotetext, currentMatch->quotelines, 5, 2, 7);

      if (tick % 60 < 30)
      {
        NF_SetTextColor(1, 0, 2);

        char helpfultext[50];
        sprintf(helpfultext, "(Touch/%s) to Fight!", btnfonts[FNT_A]);

        NF_WriteText(1, 0, 12, 16, helpfultext);
        NF_SetTextColor(1, 0, 1);
      }
    }

    if (currentScene == WINNER)
    {
      if (tick % 60 < 30)
      {
        NF_SetTextColor(1, 0, 2);

        char helpfultext[50];
        sprintf(helpfultext, "Press %s to return", btnfonts[FNT_A]);

        NF_WriteText(1, 0, 13, 21, helpfultext);
        NF_WriteText(1, 0, 4, 3, "We have a new champion!!");
        NF_SetTextColor(1, 0, 1);
      }

      NF_SetTextColor(1, 0, 1);
      for (int i = 0; i < 5; i++)
      {
        char boxerInfo[50];

        int min = (game_data.storyTimes[i + 5] / 60) / 60;
        int sec1 = ((game_data.storyTimes[i + 5] / 60) % 60) < 10
                       ? 0
                       : (game_data.storyTimes[i + 5] / 60) % 60 / 10;
        int sec2 = ((game_data.storyTimes[i + 5] / 60) % 60) % 10;
        int NA = game_data.storyTimes[i] == -1;

        switch (i)
        {
          case 0:
            sprintf(boxerInfo, NA ? "SLIM JIM     | -- -:--" : "SLIM JIM     | R%i %i:%i%i",
                    game_data.storyTimes[i], min, sec1, sec2);
            break;
          case 1:
            sprintf(boxerInfo, NA ? "JILL         | -- -:--" : "JILL         | R%i %i:%i%i",
                    game_data.storyTimes[i], min, sec1, sec2);
            break;
          case 2:
            sprintf(boxerInfo, NA ? "LIL PABLO    | -- -:--" : "LIL PABLO    | R%i %i:%i%i",
                    game_data.storyTimes[i], min, sec1, sec2);
            break;
          case 3:
            sprintf(boxerInfo, NA ? "SVETLANA     | -- -:--" : "SVETLANA     | R%i %i:%i%i",
                    game_data.storyTimes[i], min, sec1, sec2);
            break;
          case 4:
            sprintf(boxerInfo, NA ? "KAIRO        | -- -:--" : "KAIRO        | R%i %i:%i%i",
                    game_data.storyTimes[i], min, sec1, sec2);
            break;
        }
        NF_WriteText(1, 0, 5, 7 + (i * 2), boxerInfo);
      }
    }

    if (currentScene == UNDISPUTED)
    {
      if (tick % 60 < 30)
      {
        NF_SetTextColor(1, 0, 2);

        char helpfultext[50];
        sprintf(helpfultext, "Press %s to return", btnfonts[FNT_A]);

        NF_WriteText(1, 0, 13, 21, helpfultext);
        NF_WriteText(1, 0, 3, 3, "And he stays the champion!");
        NF_SetTextColor(1, 0, 1);
      }

      NF_SetTextColor(1, 0, 1);
      for (int i = 0; i < 5; i++)
      {
        char boxerInfo[50];

        int min = (game_data.TDTimes[i + 5] / 60) / 60;
        int sec1 = ((game_data.TDTimes[i + 5] / 60) % 60) < 10
                       ? 0
                       : (game_data.TDTimes[i + 5] / 60) % 60 / 10;
        int sec2 = ((game_data.TDTimes[i + 5] / 60) % 60) % 10;
        int NA = game_data.TDTimes[i] == -1;

        switch (i)
        {
          case 0:
            sprintf(boxerInfo, NA ? "SLIM JIM     | -- -:--" : "SLIM JIM     | R%i %i:%i%i",
                    game_data.TDTimes[i], min, sec1, sec2);
            break;
          case 1:
            sprintf(boxerInfo, NA ? "JILL         | -- -:--" : "JILL         | R%i %i:%i%i",
                    game_data.TDTimes[i], min, sec1, sec2);
            break;
          case 2:
            sprintf(boxerInfo, NA ? "LIL PABLO    | -- -:--" : "LIL PABLO    | R%i %i:%i%i",
                    game_data.TDTimes[i], min, sec1, sec2);
            break;
          case 3:
            sprintf(boxerInfo, NA ? "SVETLANA     | -- -:--" : "SVETLANA     | R%i %i:%i%i",
                    game_data.TDTimes[i], min, sec1, sec2);
            break;
          case 4:
            sprintf(boxerInfo, NA ? "KAIRO        | -- -:--" : "KAIRO        | R%i %i:%i%i",
                    game_data.TDTimes[i], min, sec1, sec2);
            break;
        }
        NF_WriteText(1, 0, 5, 7 + (i * 2), boxerInfo);
      }
    }
  }
  else
  {
    NF_WriteText(1, 0, 9, 9, "Paused");
    NF_WriteText(1, 0, 9, 11, "Start: Continue");
    NF_WriteText(1, 0, 9, 12, "B: Quit");
  }

  if (currentScene == INTROCUTSCENE)
  {
    if (tick % 120 == 0 && tick > 320 && tick < 1000)
    {
      mmEffectCancel(crowd);
      crowd = mmEffect(SFX_CROWD);
    }

    if (tick > 320 && tick < 760)
    {
      if (crowdVolume < 255)
        crowdVolume++;
    }
    else
    {
      if (crowdVolume > 0)
        crowdVolume--;
    }

    switch (tick)
    {
      case 10:
        intromove = -120;
        mmEffect(SFX_INTRO_ITALY);
        Transition(3, -16, 0, false);
        break;
      case 160:
        Transition(1, 0, -16, false);
        break;
      case 200:
        intromove = -120;

        mmEffect(SFX_INTRO_PAPER);
        Transition(1, -16, 0, false);
        NF_DeleteAffineBg(0, 2);
        NF_UnloadAffineBg("intro");
        NF_LoadAffineBg("bg/Intro2", "intro", 256, 256);
        NF_CreateAffineBg(0, 2, "intro", false);
        break;
      case 260:
        Transition(1, 0, -16, false);
        break;
      case 300:
        intromove = -120;

        Transition(1, -16, 0, false);
        NF_DeleteAffineBg(0, 2);
        NF_UnloadAffineBg("intro");
        NF_LoadAffineBg("bg/Intro3", "intro", 256, 256);
        NF_CreateAffineBg(0, 2, "intro", false);
        break;

      case 320:
        crowdVolume = 0;
        break;

      case 460:
        Transition(1, 0, -16, false);
        break;
      case 500:
        intromove = -120;

        Transition(1, -16, 0, false);
        NF_DeleteAffineBg(0, 2);
        NF_UnloadAffineBg("intro");
        NF_LoadAffineBg("bg/Intro4", "intro", 256, 256);
        NF_CreateAffineBg(0, 2, "intro", false);
        break;

      case 530:
        Transition(1, 3, 0, false);
        mmEffect(SFX_LIGHT);
        break;

      case 560:
        Transition(1, 3, 0, false);
        mmEffect(SFX_LIGHT);
        break;

      case 575:
        Transition(1, 3, 0, false);
        mmEffect(SFX_LIGHT);
        break;

      case 580:
        mmEffect(SFX_KO);
        break;

      case 600:
        mmEffect(SFX_BELL);
        break;

      case 620:
        mmEffect(SFX_BELL);
        break;

      case 760:
        Transition(1, 0, -16, false);
        break;
      case 800:
        intromove = 0;

        Transition(1, -16, 0, false);
        NF_DeleteAffineBg(0, 2);
        NF_UnloadAffineBg("intro");
        NF_LoadAffineBg("bg/Intro5", "intro", 256, 256);
        NF_CreateAffineBg(0, 2, "intro", false);
        break;
      case 960:
        Transition(3, 0, 16, false);
        break;
      case 1000:
        unloadIntro();
        startMainMenu(true);
        break;
      default:
        break;
    }
    setBackdropColor(RGB15(0, 0, 0));
    mmEffectVolume(crowd, crowdVolume);

    if (tick != 600)
      NF_AffineBgMove(0, 2, int(intromove / 30.0f), tick < 800 ? -16 : 0, 0);
    if (tick < 800)
      intromove++;
    NF_WriteText(1, 0, 6, 12, "Created by seaners42");
  }

  NF_UpdateTextLayers();
}
void Game::render()
{
  NF_SpriteOamSet(0);
  NF_SpriteOamSet(1);

  swiWaitForVBlank();

  oamUpdate(&oamMain);
  oamUpdate(&oamSub);
}
