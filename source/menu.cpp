#include "menu.h"
#include "enums.h"
#include "layouts.h"
#include "maxmod.h"
#include "mm_types.h"
#include "multiplayer.h"
#include "nds/arm9/background.h"
#include "nds/arm9/input.h"
#include "nds/arm9/video.h"
#include "nds/input.h"
#include "save.h"
#include "transition.h"
#include <cstdio>
#include <cstdlib>

Menu::Menu() {}

Menu::~Menu() {}

// Init menu
int Menu::Load(Menus menu)
{
  NF_LoadAffineBg("bg/Title", "title", 256, 256);
  if (game_data.wonTDOnce)
    NF_LoadAffineBg("bg/ChampMenuBg", "menu", 256, 256);
  else
    NF_LoadAffineBg("bg/MenuBg", "menu", 256, 256);

  NF_LoadSpriteGfx("sprite/buttons", 7, 64, 32);
  NF_LoadSpritePal("sprite/buttons", 7);

  NF_LoadSpriteGfx("sprite/menu-text", 8, 64, 32);
  NF_LoadSpritePal("sprite/menu-text", 8);

  NF_LoadSpriteGfx("sprite/allidles", 1, 64, 64);
  NF_LoadSpritePal("sprite/allidles", 1);

  NF_LoadSpriteGfx("sprite/cornermen", 2, 64, 64);
  NF_LoadSpritePal("sprite/cornermen", 2);

  NF_LoadSpriteGfx("sprite/rooney", 3, 64, 64);
  NF_LoadSpritePal("sprite/rooney", 3);

  NF_LoadSpriteGfx("sprite/cameron", 4, 64, 64);
  NF_LoadSpritePal("sprite/cameron", 4);

  NF_VramSpriteGfx(0, 3, 4, true);
  NF_VramSpritePal(0, 3, 4);

  NF_VramSpriteGfx(0, 4, 5, true);
  NF_VramSpritePal(0, 4, 5);

  NF_VramSpriteGfx(1, 7, 7, false);
  NF_VramSpritePal(1, 7, 7);

  NF_VramSpriteGfx(1, 8, 8, false);
  NF_VramSpritePal(1, 8, 8);

  NF_VramSpriteGfx(0, 1, 1, true);
  NF_VramSpritePal(0, 1, 1);

  NF_VramSpriteGfx(0, 1, 2, true);
  NF_VramSpritePal(0, 1, 2);

  NF_VramSpriteGfx(0, 2, 3, true);
  NF_VramSpritePal(0, 2, 3);

  NF_CreateAffineBg(1, 3, "menu", true);
  NF_CreateAffineBg(0, 3, "menu", true);
  NF_CreateAffineBg(0, 2, "title", false);

  NF_AffineBgTransform(1, 3, 256, -256, 0, 0);
  NF_AffineBgMove(1, 3, 0, 256 + 192, 0);

  NF_AffineBgMove(0, 2, 0, 0, 0);

  currentMenu = menu;
  currentSelection = NONE;
  selectionRow = -1;

  titleLoaded = true;

  menuSong = MOD_TITLETHEME;

  mmLoad(menuSong);

  mmLoadEffect(SFX_SELECT);
  mmLoadEffect(SFX_CONFIRM);

  mmSetModuleTempo(1024);
  mmSetModulePitch(1024);
  mmSetModuleVolume(game_data.muteMusic ? 0 : 256);
  mmStart(menuSong, MM_PLAY_LOOP);
  DrawLayout();

  return 0;
}

// Delete, unload, and free all menu assets
void Menu::Unload()
{
  currentMenu = NO_MENU;
  DrawLayout();

  NF_AffineBgCenter(0, 2, 0, 0);
  NF_AffineBgMove(2, 0, 0, 0, 0);
  NF_AffineBgTransform(2, 0, 256, 256, 0, 0);

  NF_AffineBgMove(3, 0, 0, 0, 0);
  NF_AffineBgMove(3, 1, 0, 0, 0);

  NF_DeleteAffineBg(1, 3);
  NF_DeleteAffineBg(0, 3);
  NF_DeleteAffineBg(0, 2);
  titleLoaded = false;

  NF_UnloadAffineBg("menu");
  NF_UnloadAffineBg("title");

  for (int unload : {7, 8})
  {
    NF_FreeSpriteGfx(1, unload);
  }

  for (int unload : {1, 2, 3, 4, 5})
  {
    NF_FreeSpriteGfx(0, unload);
  }

  for (int unload : {7, 8, 1, 2, 3, 4})
  {
    NF_UnloadSpritePal(unload);
    NF_UnloadSpriteGfx(unload);
  }
  mmStop();
  mmUnload(menuSong);
  mmUnloadEffect(SFX_SELECT);
  mmUnloadEffect(SFX_CONFIRM);

  NF_VramSpriteGfxDefrag(0);
  NF_VramSpriteGfxDefrag(1);
}

// Draw the menu layout
void Menu::DrawLayout()
{
  if (currentLayout != &LAYOUTS[0])
  {
    if (currentLayout->buttonCount > 0)
    {
      for (int i = 0; i < currentLayout->buttonCount; i++)
      {
        NF_DeleteSprite(1, 70 + i);
        for (int j = 0; j < 2; j++)
        {
          NF_DeleteSprite(1, 80 + (i * 2) + j);
        }
      }
    }
  }

  Layout* layout = GetLayoutForState(currentMenu);

  if (currentMenu == DEBUT && game_data.storyProgress <= 1)
  {
    layout = &LAYOUTS[19];
  }
  if (currentMenu == DEFENSE && game_data.TDProgress <= 6)
  {
    layout = &LAYOUTS[20];
  }

  if (layout->buttonCount > 0)
  {
    for (int i = 0; i < layout->buttonCount; i++)
    {
      int txtstagger[2] = {50 + (i * 7), 30 + (26 * i)};

      int txtcenter[2] = {94, (int)((82 + (layout->buttonCount * -10.4f)) + (26 * i))};

      // int center =

      Button button = layout->buttons[i];
      NF_CreateSprite(1, 70 + i, 8, 8,
                      (currentLayout->organization == STAGGER ? txtstagger : txtcenter)[0], // x
                      (currentLayout->organization == STAGGER ? txtstagger : txtcenter)[1]  // y
      );
      NF_SpriteFrame(1, 70 + i, button.Text);

      for (int j = 0; j < 2; j++)
      {
        int btnstagger[2] = {((j == 1) ? 20 : 84) + (i * 7), 30 + (26 * i)};

        int btncenter[2] = {(j == 1) ? 64 : 128,
                            (int)((82 + (layout->buttonCount * -10.4f)) + (26 * i))};

        NF_CreateSprite(1, 80 + (i * 2) + j, 7, 7,
                        (currentLayout->organization == STAGGER ? btnstagger : btncenter)[0],
                        (currentLayout->organization == STAGGER ? btnstagger : btncenter)[1]);

        NF_SpriteFrame(1, 80 + (i * 2) + j, j + (button.Base));
      }
    }
  }

  currentLayout = layout;
}

// All functions should be self explanatory, load/unload assets based on function called

void Menu::showGallery(bool show)
{
  if (show)
  {
    NF_LoadAffineBg(galleryBgs[pageNumber - 1], "Gallery", 256, 256);
    NF_CreateAffineBg(1, 2, "Gallery", false);
    setBackdropColorSub(RGB15(0, 3, 5));
  }
  else
  {
    {
      NF_DeleteAffineBg(1, 2);
      NF_UnloadAffineBg("Gallery");
    }
  }
}

void Menu::showRadio(bool show)
{
  if (show)
  {
    NF_LoadAffineBg("bg/Radio", "Radio", 256, 256);
    NF_CreateAffineBg(1, 2, "Radio", false);
    setBackdropColorSub(RGB15(0, 3, 5));
  }
  else
  {
    {
      NF_DeleteAffineBg(1, 2);
      NF_UnloadAffineBg("Radio");
    }
  }
}

void Menu::showFreeplayMaps(bool show)
{
  if (show)
  {
    for (int i = 0; i < 6; i++)
    {
      NF_CreateSprite(1, 60 + i, 7, 7, (i % 3) * 80 + 16, (i / 3) * 70 + 30);
      NF_SpriteFrame(1, 60 + i, i);
    }
  }
  else
  {
    for (int i = 0; i < 6; i++)
    {
      NF_DeleteSprite(1, 60 + i);
    }
  }
}

void Menu::showFreeplayBoxers(bool show)
{
  if (show)
  {
    NF_LoadAffineBg(boxersBgs[customFlags[4]], "BlueSide", 256, 256);
    NF_LoadAffineBg(boxersBgs[customFlags[5]], "RedSide", 256, 256);

    NF_CreateAffineBg(0, 2, "BlueSide", true);
    NF_CreateAffineBg(0, 3, "RedSide", true);

    NF_CreateSprite(0, 1, 1, 1, 128, 30);
    NF_EnableSpriteRotScale(0, 1, 1, true);

    NF_CreateSprite(0, 2, 2, 2, 0, 30);
    NF_EnableSpriteRotScale(0, 2, 2, true);

    NF_SpriteRotScale(0, 1, 0, 256 * 1.5, 256 * 1.5);
    NF_SpriteRotScale(0, 2, 0, -256 * 1.5, 256 * 1.5);

    windowSetBounds(WINDOW_1, 0 + 100, 0, SCREEN_WIDTH - 100, SCREEN_HEIGHT);

    bgWindowEnable(2, WINDOW_OUT);
    bgWindowEnable(3, WINDOW_OUT);
    oamWindowEnable(&oamMain, WINDOW_OUT);

    for (int i = 0; i < 100; i++)
    {
      bgWindowEnable(i, WINDOW_OUT);
      bgWindowEnable(i, WINDOW_1);
    }

    bgWindowDisable(2, WINDOW_1);
    bgWindowDisable(3, WINDOW_1);
    oamWindowEnable(&oamMain, WINDOW_1);

    windowEnable(WINDOW_1);

    setBackdropColor(RGB15(0, 3, 5));
  }
  else
  {
    NF_DeleteAffineBg(0, 2);
    NF_DeleteAffineBg(0, 3);

    NF_UnloadAffineBg("BlueSide");
    NF_UnloadAffineBg("RedSide");

    NF_DeleteSprite(0, 1);
    NF_DeleteSprite(0, 2);

    windowDisable(WINDOW_1);
  }
}

void Menu::showGuide(bool show)
{
  if (show)
  {
    game_data.lookedAtGuide = true;

    NF_LoadAffineBg("bg/Arena1", "Arena", 512, 512);
    NF_LoadAffineBg("bg/Gym", "Background", 512, 512);

    NF_CreateAffineBg(0, 2, "Arena", true);
    NF_CreateAffineBg(0, 3, "Background", true);

    int offx = 50;
    int offy = 60;
    float scale = 1.5f;

    NF_AffineBgCenter(0, 2, 0, 0);

    NF_AffineBgMove(0, 3, ((offx) + 128), ((offy) + 128), 0);
    NF_AffineBgMove(0, 2, ((offx) + 128), ((offy) + 128), 0);

    NF_AffineBgTransform(0, 3, int(256 / scale), int(256 / scale), 0, 0);
    NF_AffineBgTransform(0, 2, int(256 / scale), int(256 / scale), 0, 0);

    NF_CreateSprite(0, 5, 5, 5, 128 - 55, 30);
    NF_CreateSprite(0, 4, 4, 4, 55, 30);

    NF_EnableSpriteRotScale(0, 4, 2, true);
    NF_EnableSpriteRotScale(0, 5, 1, true);

    NF_SpriteRotScale(0, 1, 0, 256 * 1.5f, 256 * 1.5f);
    NF_SpriteRotScale(0, 2, 0, -256 * 1.5f, 256 * 1.5f);
  }
  else
  {
    NF_DeleteAffineBg(0, 2);
    NF_DeleteAffineBg(0, 3);

    NF_UnloadAffineBg("Arena");
    NF_UnloadAffineBg("Background");

    NF_DeleteSprite(0, 4);
    NF_DeleteSprite(0, 5);

    windowDisable(WINDOW_1);
  }
}

void Menu::showRecordsBoxers(bool show)
{
  if (show)
  {
    NF_CreateSprite(0, 1, 1, 1, 96, 30);
    NF_EnableSpriteRotScale(0, 1, 1, true);
    NF_SpriteRotScale(0, 1, 0, 256 * 1.5, 256 * 1.5);

    NF_CreateSprite(0, 2, 3, 3, 36, 60);
    NF_HideBg(0, 2);

    windowSetBounds(WINDOW_1, 36, 60, 36 + 64, 60 + 64);

    bgWindowEnable(2, WINDOW_OUT);
    bgWindowEnable(3, WINDOW_OUT);
    oamWindowEnable(&oamMain, WINDOW_OUT);

    for (int i = 0; i < 100; i++)
    {
      bgWindowEnable(i, WINDOW_OUT);
      bgWindowEnable(i, WINDOW_1);
    }

    bgWindowDisable(2, WINDOW_1);
    bgWindowDisable(3, WINDOW_1);
    oamWindowEnable(&oamMain, WINDOW_1);

    windowEnable(WINDOW_1);

    setBackdropColor(RGB15(0, 3, 5));
  }
  else
  {
    NF_DeleteSprite(0, 1);
    NF_DeleteSprite(0, 2);
    NF_ShowBg(0, 2);

    windowDisable(WINDOW_1);
  }
}

// Update all assets based on function called

void Menu::updateGuide()
{
  int sequence1[2][20] = {
      {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
      {0, 0, 24, 25, 26, 27, 0, 0, 24, 25, 26, 27, 0, 0, 1, 24, 25, 26, 27, 0}};
  int sequence2[2][20] = {
      {8, 8, 8, 8, 36, 37, 38, 39, 8, 8, 8, 8, 8, 8, 8, 36, 37, 38, 39, 8},
      {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}};
  int sequence3[2][20] = {{59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 0, 0, 1, 1, 2, 2, 3, 3, 0, 0},
                          {0, 0, 1, 1, 2, 24, 25, 26, 27, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 0}};

  switch (pageNumber)
  {
    case 1:
      NF_SpriteFrame(0, 4, sequence1[0][(tick / 6) % 20]);
      NF_SpriteFrame(0, 5, sequence1[1][(tick / 6) % 20]);
      break;
    case 2:
      NF_SpriteFrame(0, 4, sequence2[0][(tick / 6) % 20]);
      NF_SpriteFrame(0, 5, sequence2[1][(tick / 6) % 20]);
      break;
    case 3:
      NF_SpriteFrame(0, 4, sequence3[0][(tick / 6) % 20]);
      NF_SpriteFrame(0, 5, sequence3[1][(tick / 6) % 20]);
      break;
  }
  return;
}

void Menu::updateFreeplayBoxers()
{
  NF_AffineBgMove(0, 2, 0, tick, 0);
  NF_AffineBgMove(0, 3, 128, -tick, 0);

  int bluecorner = (pageNumber == 3 || mp_game.gamestate == 1 ? customFlags[4] + 1 : 0);
  int redcorner = (pageNumber == 3 || mp_game.gamestate == 1 ? customFlags[5] + 1 : 0);

  NF_SpriteFrame(0, 2, std::clamp((tick / 12) % 4 + (bluecorner * 4), 0, 27));

  NF_SpriteFrame(0, 1,

                 std::clamp((tick / 12) % 4 + (redcorner * 4), 0, 27));
  return;
}

void Menu::updateRecordsBoxers()
{

  NF_SpriteFrame(0, 1, std::clamp((tick / 12) % 4 + (pageNumber * 4), 0, 27));
  NF_SpriteFrame(0, 2, std::clamp((tick / 12) % 4 + ((pageNumber - 1) * 4), 0, 27));
  return;
}

void Menu::updateFreeplayMaps()
{
  for (int i = 0; i < 6; i++)
  {
    NF_SpriteFrame(1, 60 + i, i + (Cursor == i ? 6 : 0));
  }
}

// Update the general layout
void Menu::UpdateLayout()
{
  for (int i = 0; i < currentLayout->buttonCount; i++)
  {

    // Switch between these two offsets based on the organization of the layour
    int txtstagger[2] = {50 + (i * 7), 30 + (26 * i)};
    int txtcenter[2] = {94, (int)((82 + (currentLayout->buttonCount * -10.4f)) + (26 * i))};

    Button button = currentLayout->buttons[i];
    int xpos = (currentLayout->organization == STAGGER ? txtstagger : txtcenter)[0];
    int ypos = (currentLayout->organization == STAGGER ? txtstagger : txtcenter)[1];

    NF_MoveSprite(1, 70 + i,
                  xpos + sin(tick / 30.0f) * (currentSelection == button.Destination ? 5.0f : 0.0f),
                  ypos);

    {
      for (int j = 0; j < 2; j++)
      {
        int btnstagger[2] = {((j == 1) ? 20 : 84) + (i * 7), 30 + (26 * i)};

        int btncenter[2] = {(j == 1) ? 64 : 128,
                            (int)((82 + (currentLayout->buttonCount * -10.4f)) + (26 * i))};

        int xpos = (currentLayout->organization == STAGGER ? btnstagger : btncenter)[0];
        int ypos = (currentLayout->organization == STAGGER ? btnstagger : btncenter)[1];

        // Change button sprites to selected or unselected
        NF_SpriteFrame(1, 80 + (i * 2) + j,
                       j + (button.Base) + ((currentSelection == button.Destination) ? 4 : 0));

        // Sway button if selected
        NF_MoveSprite(1, 80 + (i * 2) + j,
                      xpos + sin(tick / 30.0f) *
                                 (currentSelection == button.Destination ? 5.0f : 0.0f),
                      ypos);
      }
    }
  }

  // Replace the title text on the top left if you are in multiplayer
  NF_SetTextColor(1, 0, 1);
  NF_WriteText(1, 0, 1, 1, mp_game.connected == 1 ? "Local Multiplayer" : currentLayout->menuTitle);

  char helpfultext[50];
  char left[5];
  char right[5];
  bool showQuit = false;

  // If you're in multiplayer, the B button should say quit instead of back
  // if the page you're on will make you leave

  if (mp_game.connected == 1)
  {
    if (isHost)
      showQuit = pageNumber == 1;
    else
      showQuit = pageNumber == 3;
  }

  sprintf(helpfultext, "%s: %s", btnfonts[FNT_B], showQuit ? "Quit" : "Back");
  sprintf(left, "< %s", btnfonts[FNT_LEFT]);
  sprintf(right, "%s >", btnfonts[FNT_RIGHT]);

  if (currentLayout->showBackButton)
    NF_WriteText(1, 0, 2, 21, helpfultext);

  // Show page navigation
  if (currentLayout->maxPages > 1)
  {
    NF_WriteText(1, 0, 1, 11, pageNumber > 1 ? left : "");
    NF_WriteText(1, 0, 28, 11, pageNumber < currentLayout->maxPages ? right : "");
  }

  // Sway the title and bg if it's loaded
  if (titleLoaded)
  {
    NF_AffineBgCenter(0, 2, SCREEN_WIDTH / 1.5, SCREEN_HEIGHT / 1.5);

    NF_AffineBgMove(0, 2, SCREEN_WIDTH / 1.5, SCREEN_HEIGHT / 1.5, 0);

    NF_AffineBgTransform(0, 2, (int)(256.0f + sin(tick / 30.0f) * 30.0f),
                         (int)(256.0f + cos(tick / 30.0f) * 30.0f),
                         (int)(sin(tick / 60.0f) * 30.0f), (int)(sin(tick / 60.0f) * 30.0f));

    NF_AffineBgMove(0, 3, tick / 2, (int)(sin(tick / 60.0f)) * 15.0f + 15, 0);
    NF_AffineBgMove(1, 3, -tick / 2, (int)(256 + 192 + sin(tick / 60.0f) * 15.0f + 15), 0);
  }
  else // Don't sway i guess
  {
    if (currentMenu != GUIDE) // Guide replaces the top bg, don't move it
    {
      NF_AffineBgCenter(0, 2, 0, 0);
      NF_AffineBgTransform(0, 2, 256, 256, 0, 0);
      if (currentMenu != GALLERY) // Gallery replaces the bottom bg, don't move it
        NF_AffineBgMove(1, 3, -tick / 2, 256 + 192 + sin(tick / 60.0f) * 15.0f + 15, 0);
    }
  }
}

// Constantly update based on current menu,
// I don't really need to define what each case means.

int Menu::Update()
{
  // mp packet
  pkt_client_to_host packet;

  switch (currentMenu)
  {
    case TITLE:
      if (tick % 60 > 30)
        NF_WriteText(1, 0, 9, 11, "Touch to Start");
      break;
    case MAIN:

      // Main menu stars for beating the game in story / title defense
      if (game_data.wonOnce)
        NF_WriteText(1, 0, 3, 2, btnfonts[FNT_STAR]);
      if (game_data.wonTDOnce)
        NF_WriteText(1, 0, 5, 2, btnfonts[FNT_STAR]);

      if (mp_game.connected == 1)
      {
        disable_multiplayer();
        mp_game.connected = 0;
        mp_game.gamestate = 0;
      }
      break;
    case MULTIPLAYER:
      if (mp_game.connected == 1)
      {
        disable_multiplayer();
        mp_game.connected = 0;
        mp_game.gamestate = 0;
      }
      break;

    case DEBUT:
    case DEFENSE:
      NF_SetTextColor(1, 0, 0);

      if (doubleCheck)
      {
        char helpfultext[20];

        snprintf(helpfultext, 20, "Confirm? (%s/Tap)", btnfonts[FNT_A]);

        NF_WriteText(1, 0, 8, 5, helpfultext);
      }

    case CAREER:
      char guidetext[10];

      // Show guide pointer
      if (!game_data.lookedAtGuide)
      {
        NF_SetTextColor(1, 0, 2);
        NF_WriteText(1, 0, 8, 17, "New?  Look here!");
        NF_WriteText(1, 0, 8, 15, (tick % 30 > 15) ? ">>" : " >>");
        NF_WriteText(1, 0, 21, 15, (tick % 30 > 15) ? " <<" : "<<");
        NF_SetTextColor(1, 0, 1);

        NF_SetTextColor(1, 0, 1);

        sprintf(guidetext, "%s: Guide", btnfonts[FNT_X]);
        NF_WriteText(1, 0, 12, 15, guidetext);
      }
      else
      {
        NF_SetTextColor(1, 0, 1);

        sprintf(guidetext, "%s: Guide", btnfonts[FNT_X]);
        NF_WriteText(1, 0, 22, 21, guidetext);
      }

      break;

    case MUSIC_PLAYER:
      char songname[15];

      sprintf(songname, "%i. %s", chosenSong, song_names[chosenSong - 1]);
      NF_SetTextColor(1, 0, 3);
      NF_WriteText(1, 0, 8, 8, songname);
      NF_SetTextColor(1, 0, 1);
      NF_WriteText(1, 0, 12, 13, songPlaying ? "Playing" : "Paused");
      break;

    case CREDITS:
      switch (pageNumber)
      {

        case 1:
          NF_WriteText(1, 0, 5, 7, "Art / Design");
          NF_WriteText(1, 0, 5, 10, "Sean David");
          NF_WriteText(1, 0, 5, 12, "fl4t_e4rt3r");
          break;
        case 2:
          NF_WriteText(1, 0, 5, 7, "Music");
          NF_WriteText(1, 0, 5, 10, "Lyonnel Danos");
          NF_WriteText(1, 0, 5, 12, "Camilo Hincapie");
          NF_WriteText(1, 0, 5, 14, "Alfoxed");
          break;
        case 3:
          NF_WriteText(1, 0, 5, 7, "SFX / Samples");
          NF_WriteText(1, 0, 5, 10, "Freesound.org");
          NF_WriteText(1, 0, 5, 12, "Soundbible.com");
          NF_WriteText(1, 0, 5, 14, "jsfxr");
          break;
        case 4:
          NF_WriteText(1, 0, 5, 7, "Referee Voice");
          NF_WriteText(1, 0, 5, 10, "Nashville Withrow");
          break;
        case 5:
          NF_WriteText(1, 0, 1, 4, "VAs");
          NF_WriteText(1, 0, 5, 4, "Sean David");
          NF_WriteText(1, 0, 5, 6, "   Rooney, Kairo");
          NF_WriteText(1, 0, 5, 8, "Queng Munoz");
          NF_WriteText(1, 0, 5, 10, "   Slim Jim, Lil Pablo");
          NF_WriteText(1, 0, 5, 12, "living_tragedy76");
          NF_WriteText(1, 0, 5, 14, "   Jill");
          NF_WriteText(1, 0, 5, 16, "fl4t_e4rt3r");
          NF_WriteText(1, 0, 5, 18, "   Svetlana");

          break;
        case 6:
          NF_WriteText(1, 0, 5, 7, "Extra Credits");
          NF_WriteText(1, 0, 5, 10, "William278");
          NF_WriteText(1, 0, 5, 12, "Lillian Autenreith");
          NF_WriteText(1, 0, 5, 14, "Everyone @ BlocksDS!");
          break;
        case 7:
          NF_WriteText(1, 0, 5, 5, "A Message:");
          NF_WriteText(1, 0, 5, 8, "Thank you for playing");
          NF_WriteText(1, 0, 5, 10, "my summer");
          NF_WriteText(1, 0, 5, 12, "passion project!");
          NF_WriteText(1, 0, 20, 16, "- Sean");
          break;
      }
      break;

    case GUIDE:
      char funtext[50];
      switch (pageNumber)
      {
        case 1:
          for (int i = 0; i < 2; i++)
          {

            switch (i)
            {
              case 0:
                sprintf(funtext, "%s: Light Punch", btnfonts[FNT_A]);
                break;
              case 1:
                sprintf(funtext, "%s: Heavy Punch", btnfonts[FNT_B]);
                break;
            }

            NF_WriteText(1, 0, 9, 9 + (i * 4), funtext);
          }

          break;
        case 2:
          for (int i = 0; i < 2; i++)
          {

            switch (i)
            {
              case 0:
                sprintf(funtext, "%s: Block", btnfonts[FNT_L]);
                break;
              case 1:
                sprintf(funtext, "%s: Dodge", btnfonts[FNT_R]);
                break;
            }

            NF_WriteText(1, 0, 12, 9 + (i * 4), funtext);
          }
          break;
        case 3:
          NF_DefineTextColor(1, 0, 4, 24, 20, 27);
          NF_SetTextColor(1, 0, 4);
          sprintf(funtext, "%s: The Viola", btnfonts[FNT_X]);

          NF_WriteText(1, 0, 10, 11, funtext);
          break;
      }
      updateGuide();
      break;

    case HOST_ROOM:

      tick_multiplayer();

      char helpfultext[20];

      snprintf(helpfultext, 20, "%s to Start", btnfonts[FNT_A]);

      NF_WriteText(1, 0, mp_game.connected == 1 ? 9 : 8, 8,
                   mp_game.connected == 1 ? "Player Found!" : "Finding a Player");
      NF_WriteText(1, 0, 10, 12, mp_game.connected == 1 ? helpfultext : "");

      // Timed out? Disconnect
      if (get_multiplayer_status() == MP_CONNECTION_LOST)
      {
        setMenu(MAIN);
      }

      break;

    case JOIN_ROOM:

      tick_multiplayer();

      NF_WriteText(1, 0, mp_game.connected == 1 ? 10 : 10, 9,
                   mp_game.connected == 1 ? "Room Found!" : "Searching...");

      packet.m_character = (Character)customFlags[5];

      Wifi_MultiplayerClientReplyTxFrame(&packet, sizeof(packet));

      if (get_multiplayer_status() == MP_CONNECTION_LOST)
      {
        setMenu(MAIN);
      }

      break;

    case STORY_RECORDS:
      NF_SetTextColor(1, 0, 1);
      for (int i = 0; i < 5; i++)
      {
        char boxerInfo[50];

        int time = pageNumber == 1 ? game_data.recordTimes[i + 5] : game_data.recordTD[i + 5];
        int roundnumber = pageNumber == 1 ? game_data.recordTimes[i] : game_data.recordTD[i];

        int min = (time / 60) / 60;
        int sec1 = ((time / 60) % 60) < 10 ? 0 : (time / 60) % 60 / 10;
        int sec2 = ((time / 60) % 60) % 10;
        int NA = roundnumber == -1;

        switch (i)
        {
          case 0:
            sprintf(boxerInfo, NA ? "SLIM JIM     | -- -:--" : "SLIM JIM     | R%i %i:%i%i",
                    roundnumber, min, sec1, sec2);
            break;
          case 1:
            sprintf(boxerInfo, NA ? "JILL         | -- -:--" : "JILL         | R%i %i:%i%i",
                    roundnumber, min, sec1, sec2);
            break;
          case 2:
            sprintf(boxerInfo, NA ? "LIL PABLO    | -- -:--" : "LIL PABLO    | R%i %i:%i%i",
                    roundnumber, min, sec1, sec2);
            break;
          case 3:
            sprintf(boxerInfo, NA ? "SVETLANA     | -- -:--" : "SVETLANA     | R%i %i:%i%i",
                    roundnumber, min, sec1, sec2);
            break;
          case 4:
            sprintf(boxerInfo, NA ? "KAIRO        | -- -:--" : "KAIRO        | R%i %i:%i%i",
                    roundnumber, min, sec1, sec2);
            break;
        }
        NF_WriteText(1, 0, 5, 7 + (i * 2), boxerInfo);
      }
      NF_WriteText(1, 0, 10, 4, pageNumber == 1 ? "Debut Career" : "Title Career");
      break;

    case MP_RECORDS:
      updateRecordsBoxers();
      NF_SetTextColor(1, 0, 1);
      for (int i = 0; i < 3; i++)
      {
        char mpInfo[50];
        switch (i)
        {
          case 0:
            sprintf(mpInfo, "Wins   | %i", game_data.recordMultiplayer[i][pageNumber - 1]);
            break;
          case 1:
            sprintf(mpInfo, "Losses | %i", game_data.recordMultiplayer[i][pageNumber - 1]);
            break;
          case 2:
            sprintf(mpInfo, "KOs    | %i", game_data.recordMultiplayer[i][pageNumber - 1]);
            break;
        }
        NF_WriteText(1, 0, 11, 9 + (i * 3), mpInfo);
      }
      NF_WriteText(1, 0, 11, 6, boxersText[pageNumber - 1]);
      break;

    // This is a big one
    case FREEPLAY:
    {
      if (mp_game.connected == 1)
      {
        tick_multiplayer();
        if (!isHost)
          pageNumber = 3;

        if (get_multiplayer_status() == MP_CONNECTION_LOST)
        {
          if (pageNumber == 2)
          {
            showFreeplayMaps(false);
          }
          setMenu(MAIN);
          break;
        }
      }

      updateFreeplayBoxers();

      switch (pageNumber)
      {
        case 1:

          if (mp_game.gamestate == 1 && isHost && Cursor == 0)
          {
            Cursor = 1;
          }

          char customGameText[50];

          for (int i = 0; i < 3; i++)
          {
            switch (i)
            {
              case 0:
                sprintf(customGameText, "%s:    %s %s %s ", customNames[i], Cursor == i ? ">" : " ",
                        difficultyTexts[customFlags[i]], Cursor == i ? "<" : " ");
                break;
              case 1:
                sprintf(customGameText, "%s:        %s %i %s", customNames[i],
                        Cursor == i ? ">" : " ", customFlags[i], Cursor == i ? "<" : " ");
                break;
              case 2:

                sprintf(customGameText, "%s:   %s %s %s", customNames[i], Cursor == i ? ">" : " ",
                        speedTexts[customFlags[i] - 1], Cursor == i ? "<" : " ");
                break;
            }

            if (mp_game.gamestate == 1)
            {
              if (isHost && i == 0)
                continue;
            }
            NF_SetTextColor(1, 0, Cursor == i ? 2 : 1);
            NF_WriteText(1, 0, 4, 7 + (i * 2), customGameText);
          }
          NF_SetTextColor(1, 0, 1);

          char helpfultext[50];
          sprintf(helpfultext, "%s/%s: Select", btnfonts[FNT_UP], btnfonts[FNT_DOWN]);

          NF_WriteText(1, 0, 19, 17, helpfultext);

          sprintf(helpfultext, "%s/%s: Edit", btnfonts[FNT_LEFT], btnfonts[FNT_RIGHT]);

          NF_WriteText(1, 0, 19, 19, helpfultext);

          sprintf(helpfultext, "%s: Continue", btnfonts[FNT_A]);

          NF_WriteText(1, 0, 19, 21, helpfultext);

          break;

        case 2:

          updateFreeplayMaps();
          NF_SetTextColor(1, 0, 1);
          NF_WriteText(1, 0, 18, 21, "Select A Map");
          break;

        case 3:

          if (mp_game.gamestate == 1)
          {
            Cursor = isHost ? 0 : 1;
          }

          char customBoxerText[50];
          for (int i = 0; i < 2; i++)
          {
            switch (i)
            {
              case 0:
                sprintf(customBoxerText, "Blue Corner: %s %s %s", Cursor == i ? ">" : " ",
                        boxersText[customFlags[i + 4]], Cursor == i ? "<" : " ");
                break;
              case 1:
                sprintf(customBoxerText, "Red Corner: %s  %s %s", Cursor == i ? ">" : " ",
                        boxersText[customFlags[i + 4]], Cursor == i ? "<" : " ");
                break;
            }

            NF_SetTextColor(1, 0, Cursor == i ? 2 : 1);

            if (mp_game.gamestate == 1)
            {
              if (isHost && i == 1)
                continue;

              if (!isHost && i == 0)
                continue;
            }
            NF_WriteText(1, 0, 3, mp_game.gamestate == 1 ? 11 : 9 + (i * 4), customBoxerText);
          }

          NF_SetTextColor(1, 0, 1);

          bool client = (mp_game.gamestate == 1 && !isHost);
          bool host = (mp_game.gamestate == 1 && isHost);

          NF_WriteText(1, 0, 3, 7,
                       client ? "Select Before Host Starts"
                       : host ? "Choose Your Character"
                              : "(You)");
          NF_WriteText(1, 0, 18, 21, client ? "" : "Start: Fight");
          break;
      }

      if (mp_game.connected == 1)
      {
        bool bgChange = false;

        if (isHost)
        {
          mp_game.player[0].m_character = (Character)customFlags[4];

          mp_game.chosenRounds = customFlags[1];
          mp_game.chosenTimeSpeed = customFlags[2];
          mp_game.chosenMap = customFlags[3];

          bgChange = (customFlags[5] != mp_game.player[1].m_character);
          customFlags[5] = mp_game.player[1].m_character;
        }
        else
        {
          packet.m_character = (Character)customFlags[5];

          Wifi_MultiplayerClientReplyTxFrame(&packet, sizeof(packet));

          bgChange = (customFlags[4] != mp_game.player[0].m_character);

          customFlags[1] = mp_game.chosenRounds;
          customFlags[2] = mp_game.chosenTimeSpeed;
          customFlags[3] = mp_game.chosenMap;
          customFlags[4] = mp_game.player[0].m_character;
        }

        if (bgChange)
        {
          NF_DeleteAffineBg(0, 3);
          NF_UnloadAffineBg("RedSide");
          NF_LoadAffineBg(boxersBgs[customFlags[5]], "RedSide", 256, 256);
          NF_CreateAffineBg(0, 3, "RedSide", true);

          setBackdropColor(RGB15(0, 3, 5));

          NF_DeleteAffineBg(0, 2);
          NF_UnloadAffineBg("BlueSide");
          NF_LoadAffineBg(boxersBgs[customFlags[4]], "BlueSide", 256, 256);
          NF_CreateAffineBg(0, 2, "BlueSide", true);

          setBackdropColor(RGB15(0, 3, 5));
        }
      }
      break;
    }
    case SETTINGS:
      char customGameText[50];

      for (int i = 0; i < 2; i++)
      {
        switch (i)
        {
          case 0:
            sprintf(customGameText, "Mute Music:  %s %s %s ", Cursor == i ? ">" : " ",
                    game_data.muteMusic ? "On" : "Off", Cursor == i ? "<" : " ");
            break;
          case 1:
            sprintf(customGameText, "%sReset Data%s", Cursor == i ? "> " : "",
                    Cursor == i ? " <" : " ");
            break;
        }

        if (mp_game.gamestate == 1)
        {
          if (isHost && i == 0)
            continue;
        }
        NF_SetTextColor(1, 0, Cursor == i ? 2 : 1);
        NF_WriteText(1, 0, 4, 7 + (i * 4), customGameText);
        NF_WriteText(1, 0, 4, 13, doubleCheck ? "Are you sure?" : "");
      }

      NF_SetTextColor(1, 0, 1);

      sprintf(helpfultext, "%s/%s: Select", btnfonts[FNT_UP], btnfonts[FNT_DOWN]);

      NF_WriteText(1, 0, 19, 19, helpfultext);

      sprintf(helpfultext, "%s: Edit", btnfonts[FNT_A]);

      NF_WriteText(1, 0, 19, 21, helpfultext);

      break;
    default:
      break;
  }

  if (GetLayoutForState(currentMenu) != &LAYOUTS[0])
  {
    UpdateLayout();
  }
  return 0;
}

// Based on the selection, do the action each selection wants
void Menu::buttonInterpret()
{
  switch (currentSelection)
  {
    case OPEN_CAREER:
    {
      setMenu(game_data.wonOnce ? CAREER : DEBUT);
      break;
    }

    case OPEN_DEBUT:
    {
      doubleCheck = false;
      setMenu(DEBUT);
      break;
    }

    case OPEN_DEFENSE:
    {
      doubleCheck = false;
      setMenu(DEFENSE);
      break;
    }

    case OPEN_FREEPLAY:
    {
      setMenu(FREEPLAY);
      Cursor = 0;
      break;
    }
    case OPEN_MULTIPLAER:
    {
      setMenu(MULTIPLAYER);
      break;
    }
    case OPEN_EXTRAS:
    {
      setMenu(EXTRAS);
      break;
    }
    case OPEN_RECORDS:
    {
      setMenu(RECORDS);
      break;
    }
    case OPEN_STORY_RECORDS:
    {
      setMenu(STORY_RECORDS);
      break;
    }
    case OPEN_MP_RECORDS:
    {
      setMenu(MP_RECORDS);
      break;
    }
    case OPEN_CREDITS:
    {
      setMenu(CREDITS);
      break;
    }

    case OPEN_MUSIC:
    {
      setMenu(MUSIC_PLAYER);
      break;
    }
    case OPEN_GALLERY:
    {
      setMenu(GALLERY);
      break;
    }

    case OPEN_MULTIPLAYER_HOST:
    {
      setMenu(HOST_ROOM);
      start_host();
      break;
    }
    case OPEN_MULTIPLAYER_CLIENT:
    {
      setMenu(JOIN_ROOM);
      start_client();
      break;
    }
    case OPEN_OPTIONS:
    {
      Cursor = 0;
      setMenu(SETTINGS);
      break;
    }
    default:
      break;
  }
}

// Set menu then draw layout. A lot of these layouts need different backgrounds
// so this function also handles that

void Menu::setMenu(Menus menu)
{
  previousMenu = currentMenu;
  currentMenu = menu;

  pageNumber = 1;

  if (currentLayout->needTransition || GetLayoutForState(menu)->needTransition)
  {
    Transition(3, -16, 0, true);
  }

  if (menu == MAIN || menu == MULTIPLAYER)
  {
    mp_game.gamestate = 0;
    mpStatus = MP_CONNECTION_LOST;
  }

  if (currentMenu == MP_RECORDS || previousMenu == MP_RECORDS)
  {
    showRecordsBoxers(currentMenu == MP_RECORDS);
  }

  if (currentMenu == FREEPLAY || previousMenu == FREEPLAY)
  {
    NF_FreeSpriteGfx(1, 8);

    NF_FreeSpriteGfx(1, 7);
    NF_UnloadSpriteGfx(7);
    NF_UnloadSpritePal(7);

    if (currentMenu == FREEPLAY)
    {
      NF_LoadSpriteGfx("sprite/mapselection", 7, 64, 64);
      NF_LoadSpritePal("sprite/mapselection", 7);

      NF_VramSpriteGfx(1, 7, 7, false);
      NF_VramSpritePal(1, 7, 7);
      NF_VramSpriteGfx(1, 8, 8, true);
      NF_VramSpritePal(1, 8, 8);

      NF_DeleteAffineBg(0, 3);
      NF_DeleteAffineBg(0, 2);
      titleLoaded = false;
    }
    else
    {
      showFreeplayBoxers(false);
      NF_LoadSpriteGfx("sprite/buttons", 7, 64, 32);
      NF_LoadSpritePal("sprite/buttons", 7);

      NF_VramSpriteGfx(1, 7, 7, false);
      NF_VramSpritePal(1, 7, 7);
      NF_VramSpriteGfx(1, 8, 8, false);
      NF_VramSpritePal(1, 8, 8);

      NF_CreateAffineBg(0, 3, "menu", true);
      NF_CreateAffineBg(0, 2, "title", false);
      titleLoaded = true;
    }
  }

  if (currentMenu == GUIDE || previousMenu == GUIDE)
  {
    if (currentMenu == GUIDE)
    {
      NF_DeleteAffineBg(0, 3);
      NF_DeleteAffineBg(0, 2);
      titleLoaded = false;
    }
    else
    {
      showGuide(false);
      NF_CreateAffineBg(0, 3, "menu", true);
      NF_CreateAffineBg(0, 2, "title", false);
      titleLoaded = true;
    }
  }

  if (currentMenu == GALLERY || previousMenu == GALLERY)
  {
    if (currentMenu == GALLERY)
      NF_DeleteAffineBg(1, 3);
    else
      NF_CreateAffineBg(1, 3, "menu", true);

    NF_AffineBgTransform(1, 3, 256, -256, 0, 0);
    NF_AffineBgMove(1, 3, 0, 256 + 192, 0);

    showGallery(currentMenu == GALLERY);
  }

  if (currentMenu == MUSIC_PLAYER || previousMenu == MUSIC_PLAYER)
  {
    if (currentMenu == MUSIC_PLAYER)
    {
      NF_DeleteAffineBg(1, 3);
      mmSetModuleVolume(1024);
      if (game_data.muteMusic)
      {
        songPlaying = false;
        mmPause();
      }
    }
    else
    {
      NF_CreateAffineBg(1, 3, "menu", true);
      mmSetModuleVolume(game_data.muteMusic ? 0 : 256);
    }

    NF_AffineBgTransform(1, 3, 256, -256, 0, 0);
    NF_AffineBgMove(1, 3, 0, 256 + 192, 0);

    showRadio(currentMenu == MUSIC_PLAYER);
  }

  DrawLayout();

  if (currentMenu == FREEPLAY)
  {
    showFreeplayBoxers(currentMenu == FREEPLAY);
  }

  if (currentMenu == GUIDE)
  {
    showGuide(currentMenu == GUIDE);
  }

  if (currentMenu == GALLERY)
  {
    showGallery(currentMenu == GALLERY);
  }
  if (currentMenu == MUSIC_PLAYER)
  {
    showRadio(currentMenu == MUSIC_PLAYER);
  }

  currentSelection = NONE;
  selectionRow = -1;
}

// Handle all inputs for each specific menu
MenuSelection Menu::handleInput()
{
  touchPosition touch;
  if (keysHeld() & KEY_TOUCH)
  {
    touchRead(&touch);
    touchLast[0] = touch.px;
    touchLast[1] = touch.py;
  }

  // If the menu has page navigation, this handles all of it
  if (currentLayout->maxPages > 1)
  {
    if (keysDown() & KEY_LEFT && pageNumber > 1)
    {
      pageNumber--;
      mmEffect(SFX_SELECT);
    }
    if (keysDown() & KEY_RIGHT && pageNumber < currentLayout->maxPages)
    {
      pageNumber++;
      mmEffect(SFX_SELECT);
    }

    if ((keysDown() & KEY_TOUCH &&
         isTouchInBounds(touch, 1 * 8, 10 * 8, (4 * 8), (13 * 8), false)) &&
        pageNumber > 1)
    {
      pageNumber--;
      mmEffect(SFX_SELECT);
    }
    if ((keysDown() & KEY_TOUCH &&
         isTouchInBounds(touch, 28 * 8, 10 * 8, (32 * 8), (13 * 8), false)) &&
        pageNumber < currentLayout->maxPages)
    {
      pageNumber++;
      mmEffect(SFX_SELECT);
    }
  }

  bool left_or_right_pressed = (keysDown() & KEY_LEFT || keysDown() & KEY_RIGHT);
  bool left_or_right_touched =
      keysDown() & KEY_TOUCH &&
      ((isTouchInBounds(touch, 1 * 8, 10 * 8, (4 * 8), (13 * 8), false)) ||
       (isTouchInBounds(touch, 28 * 8, 10 * 8, (32 * 8), (13 * 8), false)));

  switch (currentMenu)
  {

    // Enter game
    case TITLE:
      if (keysUp() & KEY_TOUCH || keysDown() & KEY_A || keysDown() & KEY_START)
      {
        mmEffect(SFX_CONFIRM);
        setMenu(MAIN);
      }

    // Start MP
    case HOST_ROOM:

      if ((keysDown() & KEY_A) && mp_game.connected == 1)
      {
        mp_game.gamestate = 1;
        mmEffect(SFX_CONFIRM);
        setMenu(FREEPLAY);
      }
      break;

    // Did the host start MP?
    case JOIN_ROOM:

      if (mp_game.gamestate == 1 && mp_game.connected == 1)
      {
        setMenu(FREEPLAY);
      }
      break;

    // Show guide
    case DEBUT:
    case DEFENSE:
    case CAREER:
      if (keysDown() & KEY_X || (keysDown() & KEY_TOUCH &&
                                 isTouchInBounds(touch, 22 * 8, 21 * 8, (30 * 8), (22 * 8), false)))
      {
        mmEffect(SFX_CONFIRM);
        setMenu(GUIDE);
      }
      break;

    // Scroll through the gallery
    case GALLERY:

      if ((left_or_right_touched || left_or_right_pressed) && (pageNumber >= 1 && pageNumber <= 7))
      {
        Transition(2, -16, 0, true);

        NF_DeleteAffineBg(1, 2);
        NF_UnloadAffineBg("Gallery");

        mmEffect(SFX_SELECT);
        showGallery(true);
      }
      break;

    // Scroll through the music and pause if you'd like
    case MUSIC_PLAYER:
      if ((keysDown() & KEY_LEFT ||
           (keysDown() & KEY_TOUCH && isTouchInBounds(touch, 84, 123, 101, 141, false))) &&
          chosenSong > 1)
      {
        chosenSong--;
        mmEffect(SFX_SELECT);
        mmStop();
        mmUnload(menuSong);
        menuSong = mus_player[chosenSong - 1];
        mmLoad(menuSong);
        mmStart(menuSong, MM_PLAY_LOOP);
        mmSetModuleVolume(1024);
      }
      if ((keysDown() & KEY_RIGHT ||
           (keysDown() & KEY_TOUCH && isTouchInBounds(touch, 154, 123, 172, 141, false))) &&
          chosenSong < 10)
      {
        chosenSong++;
        mmEffect(SFX_SELECT);
        mmStop();
        mmUnload(menuSong);
        menuSong = mus_player[chosenSong - 1];
        mmLoad(menuSong);
        mmStart(menuSong, MM_PLAY_LOOP);
        mmSetModuleVolume(1024);
      }
      if (keysDown() & KEY_A ||
          (keysDown() & KEY_TOUCH && isTouchInBounds(touch, 118, 123, 138, 141, false)))
      {
        mmEffect(SFX_CONFIRM);
        songPlaying = !songPlaying;
      }

      if (!songPlaying)
        mmPause();
      else
        mmResume();

      break;
      // Select and edit the only two options in settings. I was gonna make keybindings but
      // It seemed like too much

    case SETTINGS:
      if (keysDown() & KEY_DOWN && Cursor < 1)
      {
        Cursor = Cursor + 1;
        doubleCheck = false;
        mmEffect(SFX_SELECT);
      }

      if (keysDown() & KEY_UP && Cursor > 0)
      {
        Cursor = Cursor - 1;
        doubleCheck = false;
        mmEffect(SFX_SELECT);
      }

      if (keysDown() & KEY_A)
      {
        switch (Cursor)
        {
          case 0:
            mmEffect(SFX_CONFIRM);
            game_data.muteMusic = !game_data.muteMusic;
            mmSetModuleVolume(game_data.muteMusic ? 0 : 256);
            break;
          case 1:
            if (doubleCheck)
            {
              mmEffect(SFX_CONFIRM);
              doubleCheck = false;
              reset_data();
              Unload();
              Transition(3, -16, 0, true);
              Load(TITLE);
            }
            else
            {
              doubleCheck = true;
            }
        }
      }

      break;

    case FREEPLAY:
      // Each freeplay menu treats input differently.
      // Also multiplayer uses this case so a lot of interweaving happens here,
      // Probably wasn't a good idea but it made sense at the time

      switch (pageNumber)
      {
        // Select difficulty, round number, and time speed
        case 1:

          if (keysDown() & KEY_DOWN && Cursor < 2)
          {
            Cursor = Cursor + 1;
            mmEffect(SFX_SELECT);
          }

          if (keysDown() & KEY_UP && Cursor > (mp_game.gamestate == 1 ? 1 : 0))
          {
            Cursor = Cursor - 1;
            mmEffect(SFX_SELECT);
          }

          if (keysDown() & KEY_RIGHT)
          {
            mmEffect(SFX_CONFIRM);
            customFlags[Cursor] = std::clamp(customFlags[Cursor] + customValues[Cursor][2],
                                             customValues[Cursor][0], customValues[Cursor][1]);
          }

          if (keysDown() & KEY_LEFT)
          {
            mmEffect(SFX_CONFIRM);
            customFlags[Cursor] = std::clamp(customFlags[Cursor] - customValues[Cursor][2],
                                             customValues[Cursor][0], customValues[Cursor][1]);
          }

          break;

        // Select map
        case 2:

          if (keysDown() & KEY_RIGHT && ((Cursor >= 0 && Cursor < 2) || (Cursor > 2 && Cursor < 5)))
          {
            mmEffect(SFX_SELECT);
            Cursor = Cursor + 1;
          }

          if (keysDown() & KEY_LEFT && ((Cursor > 0 && Cursor <= 2) || (Cursor > 3 && Cursor <= 5)))
          {
            mmEffect(SFX_SELECT);
            Cursor = Cursor - 1;
          }

          if (keysDown() & KEY_DOWN && Cursor < 3)
          {
            mmEffect(SFX_SELECT);
            Cursor = Cursor + 3;
          }

          if (keysDown() & KEY_UP && Cursor >= 3)
          {
            mmEffect(SFX_SELECT);
            Cursor = Cursor - 3;
          }

          if (keysDown() & KEY_TOUCH)
          {
            mmEffect(SFX_SELECT);
            for (int i = 0; i < 6; i++)
            {
              int x = (i % 3) * 80 + 16;
              int y = (i / 3) * 70 + 30;
              if (isTouchInBounds(touch, x, y, x + 64, y + 64, false))
              {
                if (customFlags[3] == i)
                {
                  mmEffect(SFX_CONFIRM);
                  if (pageNumber == 2)
                    showFreeplayMaps(false);

                  pageNumber = pageNumber + 1;
                  Cursor = pageNumber == 2 ? customFlags[3] : 0;
                  Transition(2, -16, 0, false);
                  break;
                }
                Cursor = i;
              }
            }
          }

          if (keysUp() & KEY_TOUCH)
          {
            for (int i = 0; i < 6; i++)
            {
              int x = (i % 3) * 80 + 16;
              int y = (i / 3) * 70 + 30;

              if (isTouchInBounds(touch, x, y, x + 64, y + 64, true))
              {
                if (customFlags[3] == i)
                {
                  mmEffect(SFX_CONFIRM);
                  if (pageNumber == 2)
                    showFreeplayMaps(false);

                  pageNumber = pageNumber + 1;
                  Cursor = pageNumber == 2 ? customFlags[3] : 0;
                  Transition(2, -16, 0, false);
                  break;
                }
              }
            }
          }

          customFlags[3] = Cursor;

          break;

        // Select characters
        case 3:

          if (mp_game.connected != 1)
          {
            if (keysDown() & KEY_DOWN && Cursor < 1)
            {
              mmEffect(SFX_SELECT);
              Cursor = Cursor + 1;
            }

            if (keysDown() & KEY_UP && Cursor > 0)
            {
              mmEffect(SFX_SELECT);
              Cursor = Cursor - 1;
            }
          }

          if (keysDown() & KEY_RIGHT)
          {
            bool changeBg = (customFlags[Cursor + 4] != 5);

            customFlags[Cursor + 4] =
                std::clamp(customFlags[Cursor + 4] + customValues[Cursor][2],
                           customValues[Cursor + 4][0], customValues[Cursor + 4][1]);

            if (changeBg)
            {
              mmEffect(SFX_CONFIRM);
              NF_DeleteAffineBg(0, 3);
              NF_UnloadAffineBg("RedSide");
              NF_LoadAffineBg(boxersBgs[customFlags[5]], "RedSide", 256, 256);
              NF_CreateAffineBg(0, 3, "RedSide", true);

              setBackdropColor(RGB15(0, 3, 5));

              NF_DeleteAffineBg(0, 2);
              NF_UnloadAffineBg("BlueSide");
              NF_LoadAffineBg(boxersBgs[customFlags[4]], "BlueSide", 256, 256);
              NF_CreateAffineBg(0, 2, "BlueSide", true);

              setBackdropColor(RGB15(0, 3, 5));

              Transition(1, 5, 0, false);
              setBrightness(2, 0);
            }
          }

          if (keysDown() & KEY_LEFT)
          {
            bool changeBg = (customFlags[Cursor + 4] != 0);

            customFlags[Cursor + 4] =
                std::clamp(customFlags[Cursor + 4] - customValues[Cursor][2],
                           customValues[Cursor + 4][0], customValues[Cursor + 4][1]);

            if (changeBg)
            {
              mmEffect(SFX_CONFIRM);
              NF_DeleteAffineBg(0, 3);
              NF_UnloadAffineBg("RedSide");
              NF_LoadAffineBg(boxersBgs[customFlags[5]], "RedSide", 256, 256);
              NF_CreateAffineBg(0, 3, "RedSide", true);

              setBackdropColor(RGB15(0, 3, 5));

              NF_DeleteAffineBg(0, 2);
              NF_UnloadAffineBg("BlueSide");
              NF_LoadAffineBg(boxersBgs[customFlags[4]], "BlueSide", 256, 256);
              NF_CreateAffineBg(0, 2, "BlueSide", true);

              setBackdropColor(RGB15(0, 3, 5));

              setBrightness(2, 0);
              Transition(1, 5, 0, false);
            }
          }

          break;
      }

      // A: Move onto the next menu
      if (keysDown() & KEY_A && pageNumber < 3)
      {
        mmEffect(SFX_CONFIRM);
        if (pageNumber == 2)
          showFreeplayMaps(false);

        pageNumber = pageNumber + 1;
        Cursor = pageNumber == 2 ? customFlags[3] : 0;
        Transition(2, -16, 0, false);

        if (pageNumber == 2)
          showFreeplayMaps(true);
      }

      // B: or Back Button Touched: Move onto the previous menu or quit
      if (keysDown() & KEY_B || (keysDown() & KEY_TOUCH &&
                                 isTouchInBounds(touch, 2 * 8, 21 * 8, (9 * 8), (22 * 8), false)))
      {
        mmEffect(SFX_CONFIRM);
        if (mp_game.gamestate == 1 && !isHost)
        {
          setMenu(MAIN);
          break;
        }

        if (pageNumber == 2)
          showFreeplayMaps(false);

        if (pageNumber == 1)
        {
          setMenu(MAIN);
          break;
        }

        pageNumber = pageNumber > 1 ? pageNumber - 1 : pageNumber;
        Cursor = pageNumber == 2 ? customFlags[3] : 0;
        Transition(2, -16, 0, false);

        if (pageNumber == 2)
        {
          setBrightness(1, 0);
          showFreeplayMaps(true);
        }
      }

      // Start the fight
      if (keysDown() & KEY_START && pageNumber == 3)
      {
        mmEffect(SFX_CONFIRM);
        if (mp_game.connected == 1)
        {
          if (isHost)
          {
            mp_game.gamestate = 2;
            SendHostStateToClients();
            return START_MULTIPLAYER;
          }
          return NONE;
        }
        return START_FREEPLAY;
      }

      // Am I the client?
      if (mp_game.connected == 1 && (!isHost))
      {
        if (mp_game.gamestate == 2)
        {
          return START_MULTIPLAYER;
        }
      }

      break;
    default:
      break;
  }

  return handleLayoutInput(touch);
}

// Detects if the DS touch was within a menu button
bool Menu::isTouchOnButton(int row, touchPosition touch, bool up)
{
  int btnstagger[2] = {20 + (row * 7), 30 + (26 * row)};

  int btncenter[2] = {64, (int)((82 + (currentLayout->buttonCount * -10.4f)) + (26 * row))};

  int xbounds[2] = {(currentLayout->organization == STAGGER ? btnstagger : btncenter)[0],
                    (currentLayout->organization == STAGGER ? btnstagger : btncenter)[0] + 128};
  int ybounds[2] = {(currentLayout->organization == STAGGER ? btnstagger : btncenter)[1],
                    (currentLayout->organization == STAGGER ? btnstagger : btncenter)[1] + 24};

  if (up)
  {
    return (touchLast[0] >= xbounds[0] && touchLast[0] <= xbounds[1] &&
            touchLast[1] >= ybounds[0] && touchLast[1] <= ybounds[1]);
  }
  else
  {
    return (touch.px >= xbounds[0] && touch.px <= xbounds[1] && touch.py >= ybounds[0] &&
            touch.py <= ybounds[1]);
  }
}

// Detects if the DS touch was within specified bounds
bool Menu::isTouchInBounds(touchPosition touch, int x1, int y1, int x2, int y2, bool up)
{
  if (up)
  {
    return (touchLast[0] >= x1 && touchLast[0] <= x2 && touchLast[1] >= y1 && touchLast[1] <= y1);
  }
  else
  {
    return (touch.px >= x1 && touch.px <= x2 && touch.py >= y1 && touch.py <= y2);
  }
}

// customFlags
int* Menu::getCustomGame()
{
  return customFlags;
}

// Handle input for all general layout
MenuSelection Menu::handleLayoutInput(touchPosition touch)
{
  // Button touched?
  if (keysDown() & KEY_TOUCH)
  {
    touchRead(&touch);
    for (int i = 0; i < currentLayout->buttonCount; i++)
    {
      Button button = currentLayout->buttons[i];
      if (isTouchOnButton(i, touch, false))
      {
        currentSelection = button.Destination;
        selectionRow = i;
        mmEffect(SFX_SELECT);
        return NONE;
      }
    }
  }

  // On touch up, only return the selection if it was the same one on touch down
  if (keysUp() & KEY_TOUCH)
  {
    for (int i = 0; i < currentLayout->buttonCount; i++)
    {
      Button button = currentLayout->buttons[i];
      if (isTouchOnButton(i, touch, true) && (button.Destination == currentSelection))
      {
        buttonInterpret();
        if ((currentSelection == START_NEW_GAME || currentSelection == START_NEW_TD) &&
            !doubleCheck)
        {
          if ((currentMenu == DEBUT && game_data.storyProgress <= 1) ||
              (currentMenu == DEFENSE && game_data.TDProgress <= 6))
          {
            mmEffect(SFX_CONFIRM);
            return currentSelection;
          }
          doubleCheck = true;
          return NONE;
        }
        mmEffect(SFX_CONFIRM);
        return currentSelection;
      }
    }
    currentSelection = NONE;
    selectionRow = -1;
    return NONE;
  }

  // hi!
  // Confirm your selection
  if (keysDown() & KEY_A || keysDown() & KEY_START)
  {
    buttonInterpret();

    // New game? You should be absolutely sure
    if ((currentSelection == START_NEW_GAME || currentSelection == START_NEW_TD) && !doubleCheck)
    {
      if ((currentMenu == DEBUT && game_data.storyProgress <= 1) ||
          (currentMenu == DEFENSE && game_data.TDProgress <= 6))
      {
        mmEffect(SFX_CONFIRM);
        return currentSelection;
      }
      doubleCheck = true;
      return NONE;
    }
    mmEffect(SFX_CONFIRM);
    return currentSelection;
  }

  // Anything else pressed cancels the double check
  if (keysDown())
  {
    doubleCheck = false;
  }

  // B for back
  if (keysUp() & KEY_B && (currentMenu != FREEPLAY) && (brightness == 0))
  {
    if (currentLayout->backWhere == MAIN || currentLayout->backWhere == MULTIPLAYER)
    {
      disable_multiplayer();
    }
    mmEffect(SFX_CONFIRM);
    setMenu((currentLayout->backWhere == CAREER && !game_data.wonOnce) ? MAIN
                                                                       : currentLayout->backWhere);
  }

  // Did you touch the back button instead? Fancy
  if (currentLayout->showBackButton &&
      (keysDown() & KEY_TOUCH && isTouchInBounds(touch, 2 * 8, 21 * 8, (9 * 8), (22 * 8), false)) &&
      (currentMenu != FREEPLAY) && (brightness == 0))
  {
    if (currentLayout->backWhere == MAIN || currentLayout->backWhere == MULTIPLAYER)
    {
      disable_multiplayer();
    }
    mmEffect(SFX_CONFIRM);
    setMenu((currentLayout->backWhere == CAREER && !game_data.wonOnce) ? MAIN
                                                                       : currentLayout->backWhere);
  }

  // If your menu uses a custom navigation (Pages, freeplay config, settings) don't do the stuff
  // below

  // This is because pressing A after using the DPad on a menu with no buttons just takes you
  // to the Career menu

  if (currentLayout->customNavigation)
  {
    return NONE;
  }

  // Navigate buttons
  if (keysDown() & KEY_DOWN)
  {
    selectionRow = std::min(selectionRow + 1, currentLayout->buttonCount - 1);
    currentSelection = currentLayout->buttons[selectionRow].Destination;
    mmEffect(SFX_SELECT);
  }

  if (keysDown() & KEY_UP)
  {
    selectionRow = std::max(selectionRow - 1, 0);
    currentSelection = currentLayout->buttons[selectionRow].Destination;
    mmEffect(SFX_SELECT);
  }

  return NONE;
}
