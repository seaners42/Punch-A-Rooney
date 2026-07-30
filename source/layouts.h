#pragma once

#include "enums.h"

struct Button
{
  const int Base;
  const int Text;
  const MenuSelection Destination;
};

struct Layout
{
  const int buttonCount;
  const Button buttons[5];
  const bool needTransition;
  const bool showBackButton;
  const char menuTitle[30];
  const Menus backWhere;
  const MenuOrganization organization;
  const bool customNavigation;
  const int maxPages;
};

static Layout LAYOUTS[22] = {
    {
        0,
        {},
        false,
        false,
        "",
        MAIN,
        NO_ORG,
        false,
    },
    {
        5,
        {{0, 0, OPEN_CAREER},
         {0, 1, OPEN_FREEPLAY},
         {0, 2, OPEN_MULTIPLAER},
         {0, 5, OPEN_EXTRAS},
         {2, 6, OPEN_OPTIONS}},
        false,
        false,
        "",
        TITLE,
        STAGGER,
        false,
    },
    {2,
     {
         {0, 7, START_NEW_GAME},
         {0, 8, START_PREV_GAME},
     },
     false,
     true,
     "Debut Career",
     CAREER,
     CENTERED,
     false},
    {0, {}, true, true, "Freeplay Mode", CAREER, NO_ORG, true},
    {2,
     {
         {0, 4, OPEN_MULTIPLAYER_HOST},
         {0, 3, OPEN_MULTIPLAYER_CLIENT},
     },
     false,
     true,
     "Local Multiplayer",
     MAIN,
     CENTERED,
     false},
    {4,
     {
         {0, 9, OPEN_MUSIC},
         {0, 10, OPEN_GALLERY},
         {0, 11, OPEN_RECORDS},
         {2, 12, OPEN_CREDITS},
     },
     false,
     true,
     "Extras Menu",
     MAIN,
     CENTERED,
     false},
    {0, {}, false, true, "Options", MAIN, NO_ORG, true},
    {0, {}, true, true, "Hosting a room...", MULTIPLAYER, NO_ORG, true},
    {0, {}, true, true, "Finding a room...", MULTIPLAYER, NO_ORG, true},
    {2,
     {
         {0, 0, OPEN_STORY_RECORDS},
         {0, 15, OPEN_MP_RECORDS},
     },
     false,
     true,
     "Records",
     EXTRAS,
     CENTERED,
     false},
    {0, {}, false, true, "Credits", EXTRAS, NO_ORG, true, 7},
    {0, {}, false, true, "Story Records", RECORDS, NO_ORG, true, 2},
    {0, {}, false, true, "Multiplayer Records", RECORDS, NO_ORG, true, 6},
    {0, {}, true, true, "", EXTRAS, NO_ORG, true},
    {0, {}, true, true, "Gallery", EXTRAS, NO_ORG, true, 7},
    {2,
     {
         {0, 7, START_NEW_TD},
         {0, 8, START_PREV_TD},
     },
     false,
     true,
     "Title Defense Career",
     CAREER,
     CENTERED,
     false},
    {2,
     {
         {0, 13, OPEN_DEBUT},
         {0, 14, OPEN_DEFENSE},
     },
     false,
     true,
     "Career",
     MAIN,
     CENTERED,
     false},
    {0, {}, true, true, "Guide", CAREER, CENTERED, true, 3},
    {0, {}, false, false, "", TITLE, CENTERED, true},
    {1,
     {
         {0, 7, START_NEW_GAME},
     },
     false,
     true,
     "Debut Career",
     CAREER,
     CENTERED,
     false},
    {1,
     {
         {0, 7, START_NEW_TD},
     },
     false,
     true,
     "Title Defense Career",
     CAREER,
     CENTERED,
     false},
    {1,
     {
         {0, 12, OPEN_CREDITS},
     },
     false,
     false,
     "",
     WONTD,
     CENTERED,
     false},

};

static Layout* GetLayoutForState(Menus state)
{
  switch (state)
  {
    case MAIN:
      return &LAYOUTS[1];
      break;
    case DEBUT:
      return &LAYOUTS[2];
      break;
    case FREEPLAY:
      return &LAYOUTS[3];
      break;
    case MULTIPLAYER:
      return &LAYOUTS[4];
      break;
    case EXTRAS:
      return &LAYOUTS[5];
      break;
    case SETTINGS:
      return &LAYOUTS[6];
      break;
    case HOST_ROOM:
      return &LAYOUTS[7];
      break;
    case JOIN_ROOM:
      return &LAYOUTS[8];
      break;
    case RECORDS:
      return &LAYOUTS[9];
      break;
    case CREDITS:
      return &LAYOUTS[10];
      break;
    case STORY_RECORDS:
      return &LAYOUTS[11];
      break;
    case MP_RECORDS:
      return &LAYOUTS[12];
      break;
    case MUSIC_PLAYER:
      return &LAYOUTS[13];
      break;
    case GALLERY:
      return &LAYOUTS[14];
      break;
    case DEFENSE:
      return &LAYOUTS[15];
      break;
    case CAREER:
      return &LAYOUTS[16];
      break;
    case GUIDE:
      return &LAYOUTS[17];
      break;
    case TITLE:
      return &LAYOUTS[18];
      break;
    case WONTD:
      return &LAYOUTS[21];
      break;
    default:
      return &LAYOUTS[0];
      break;
  }
}