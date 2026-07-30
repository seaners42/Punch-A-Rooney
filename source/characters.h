#pragma once
#include "enums.h"
#include "maxmod9.h"
#include "soundbank.h"

struct traits
{
  int idleframes[2];
  int walkframes[2];
  int blockframes[2];

  int lightframes1[2];
  int lightframes2[2];
  int heavyframes[2];
  int specialframes[2];

  int tiredframes[2];
  int dodgeframes[2];

  int dmgframes[2];
  int downframes[2];
  int getupframes[2];

  int cellyframes[2];

  int whenlight[2];
  int whenheavy;
  int whenspecial;

  int walkspeed;
  int punchspeed;
  int combospeed;
  int dodgespeed;

  int lightdmg;
  int heavydmg;

  int dodgedistance;
  int punchdist;
  int maxFrame;
};

struct superframe
{
  int frame;
  int when;
  int damage;
  bool lock;
  int kb;
  int sfx;
};

struct superSequence
{
  superframe sequence[50];
};

static traits chars[6] = {{
                              {0, 3},  // idle
                              {4, 7},  // walk
                              {8, 11}, // block

                              {12, 16}, // light
                              {17, 21}, // light2
                              {22, 31}, // heavy
                              {59, 68}, // special

                              {32, 35}, // tired
                              {36, 38}, // dodge

                              {40, 43}, // dmg
                              {44, 52}, // down
                              {53, 54}, // getup

                              {55, 58}, // celebration

                              {14, 19}, // light frames that cause damage
                              27,       // heavy frame that cause damage
                              64,       // special frame that cause damage

                              12, // walk speed
                              6,  // punch speed
                              5,  // combo speed
                              6,  // dodge speed

                              5,  // light damage
                              15, // heavy damage

                              0,  // dodge distance
                              25, // punch distance
                              68, // maxframe
                          },
                          {
                              {0, 3},  // idle
                              {4, 7},  // walk
                              {8, 11}, // block

                              {12, 16}, // light
                              {17, 21}, // light2
                              {22, 31}, // heavy
                              {32, 41}, // special

                              {42, 45}, // tired
                              {46, 48}, // dodge

                              {50, 53}, // dmg
                              {54, 63}, // down
                              {64, 65}, // getup

                              {66, 69}, // celebration

                              {14, 19}, // light frames that cause damage
                              27,       // heavy frame that cause damage
                              37,       // special frame that cause damage

                              9,  // walk speed
                              7,  // punch speed
                              6,  // combo speed
                              10, // dodge speed

                              5,  // light damage
                              15, // heavy damage

                              5,  // dodge distance
                              28, // punch distance
                              69, // maxframe
                          },
                          {
                              {0, 3},  // idle
                              {4, 7},  // walk
                              {8, 11}, // block

                              {12, 16}, // light
                              {17, 21}, // light2
                              {22, 31}, // heavy
                              {32, 41}, // special

                              {42, 45}, // tired
                              {46, 48}, // dodge

                              {49, 52}, // dmg
                              {53, 63}, // down
                              {64, 65}, // getup

                              {66, 69}, // celebration

                              {14, 19}, // light frames that cause damage
                              27,       // heavy frame that cause damage
                              37,       // special frame that cause damage

                              12, // walk speed
                              6,  // punch speed
                              5,  // combo speed
                              6,  // dodge speed

                              5,  // light damage
                              15, // heavy damage

                              5,  // dodge distance
                              25, // punch distance
                              69, // maxframe
                          },
                          {
                              {0, 3},  // idle
                              {4, 7},  // walk
                              {8, 11}, // block

                              {12, 16}, // light
                              {17, 21}, // light2
                              {22, 30}, // heavy
                              {31, 39}, // special

                              {46, 49}, // tired
                              {50, 52}, // dodge

                              {54, 57}, // dmg
                              {58, 66}, // down
                              {67, 68}, // getup

                              {69, 72}, // celebration

                              {14, 19}, // light frames that cause damage
                              27,       // heavy frame that cause damage
                              36,       // special frame that cause damage

                              9, // walk speed
                              5, // punch speed
                              4, // combo speed
                              6, // dodge speed

                              3,  // light damage
                              10, // heavy damage

                              -5, // dodge distance
                              23, // punch distance
                              72, // maxframe
                          },
                          {
                              {0, 3},  // idle
                              {4, 7},  // walk
                              {8, 11}, // block

                              {16, 21}, // light
                              {22, 27}, // light2
                              {28, 37}, // heavy
                              {38, 46}, // special

                              {47, 50}, // tired
                              {51, 53}, // dodge

                              {55, 58}, // dmg
                              {59, 69}, // down
                              {70, 71}, // getup

                              {72, 75}, // celebration

                              {19, 25}, // light frames that cause damage
                              33,       // heavy frame that cause damage
                              43,       // special frame that cause damage

                              12, // walk speed
                              7,  // punch speed
                              5,  // combo speed
                              6,  // dodge speed

                              8,  // light damage
                              20, // heavy damage

                              0,  // dodge distance
                              25, // punch distance
                              75, // maxframe
                          },
                          {
                              {4, 7},   // idle
                              {12, 15}, // walk
                              {20, 23}, // block

                              {37, 41}, // light
                              {42, 46}, // light2
                              {47, 56}, // heavy
                              {57, 66}, // special

                              {67, 70}, // tired
                              {72, 74}, // dodge

                              {75, 77}, // dmg
                              {78, 85}, // down
                              {86, 87}, // getup

                              {88, 91}, // celebration

                              {39, 44}, // light frames that cause damage
                              52,       // heavy frame that cause damage
                              62,       // special frame that cause damage

                              12, // walk speed
                              6,  // punch speed
                              4,  // combo speed
                              6,  // dodge speed

                              5,  // light damage
                              18, // heavy damage

                              5,  // dodge distance
                              25, // punch distance
                              91, // maxframe
                          }};

static superSequence super[6] = {{{
                                     {64, 0}, // ROONEY
                                     {64, 1},
                                     {67, 6},
                                     {68, 18},
                                     {0, 36},
                                     {12, 50 + (5 * 3) - 6},
                                     {13, 50 + (5 * 3) - 3},
                                     {14, 50 + (5 * 3), 7, true, 5, SFX_RN_SUPER1},
                                     {15, 50 + (5 * 5)},
                                     {16, 50 + (5 * 6)},
                                     {12, 50 + (5 * 9) - 6, -2},
                                     {13, 50 + (5 * 9) - 3},
                                     {14, 50 + (5 * 9), 8, true, 5, SFX_RN_SUPER2},
                                     {15, 50 + (5 * 10)},
                                     {16, 50 + (5 * 11)},
                                     {36, 115 + (6 * 1), -2},
                                     {37, 115 + (6 * 2)},
                                     {38, 115 + (6 * 3)},
                                     {39, 115 + (6 * 8) - 7},
                                     {26, 115 + (6 * 9) - 3},
                                     {27, 115 + (6 * 10), 20, true, 7, SFX_RN_SUPER3},
                                     {28, 115 + (6 * 12)},
                                     {29, 115 + (6 * 13)},
                                     {30, 115 + (6 * 14)},
                                     {31, 115 + (6 * 15), -1, false},
                                 }},
                                 {{
                                     {37, 0}, // SLIM JIM
                                     {38, 1},
                                     {39, 6},
                                     {40, 12, 0, true, 0, SFX_SJ_SUPER},
                                     {66, 18},
                                     {67, 24},
                                     {68, 30},
                                     {69, 36},
                                     {66, 42},
                                     {12, 90 + (5 * 1)},
                                     {13, 90 + (5 * 2)},
                                     {14, 90 + (5 * 3), 3, true, 5},
                                     {15, 90 + (5 * 4)},
                                     {16, 90 + (5 * 5)},
                                     {17, 90 + (5 * 6), -2},
                                     {18, 90 + (5 * 7)},
                                     {19, 90 + (5 * 8), 4, true, 5},
                                     {20, 90 + (5 * 9)},
                                     {21, 90 + (5 * 10)},
                                     {12, 90 + (5 * 11), -2},
                                     {13, 90 + (5 * 12)},
                                     {14, 90 + (5 * 13), 4, true, 5},
                                     {15, 90 + (5 * 14)},
                                     {16, 90 + (5 * 15)},
                                     {17, 90 + (5 * 16), -2},
                                     {18, 90 + (5 * 17)},
                                     {24, 90 + (5 * 18), 4, true, 5},
                                     {25, 180 + (6 * 1)},
                                     {26, 180 + (6 * 3)},
                                     {27, 180 + (6 * 4), 20, true, 5},
                                     {28, 180 + (6 * 5)},
                                     {29, 180 + (6 * 6)},
                                     {30, 180 + (6 * 7)},
                                     {31, 180 + (6 * 8), -1, false},
                                 }},
                                 {{
                                     {37, 0}, // JILL
                                     {38, 1},
                                     {39, 6},
                                     {40, 12},
                                     {22, 18},
                                     {23, 24},
                                     {24, 30},
                                     {25, 36},
                                     {15, 50 + (2 * 1), 2, true, 2},
                                     {16, 50 + (2 * 2)},
                                     {19, 50 + (2 * 3), 2, true, 2},
                                     {18, 50 + (2 * 4), -2},

                                     {15, 50 + (2 * 5), 2, true, 2},
                                     {16, 50 + (2 * 6)},
                                     {19, 50 + (2 * 7), 1, true, 2},
                                     {18, 50 + (2 * 8), -2},

                                     {15, 50 + (2 * 9), 2, true, 2},
                                     {16, 50 + (2 * 10)},
                                     {19, 50 + (2 * 11), 2, true, 2},
                                     {18, 50 + (2 * 12), -2},

                                     {15, 50 + (2 * 13), 2, true, 2},
                                     {16, 50 + (2 * 14)},
                                     {19, 50 + (2 * 15), 2, true, 2},
                                     {18, 50 + (2 * 16), -2},

                                     {25, 82 + (6 * 0)},
                                     {26, 82 + (6 * 1)},
                                     {27, 82 + (6 * 2), 20, true, 5},
                                     {28, 82 + (6 * 3)},
                                     {29, 82 + (6 * 4)},
                                     {30, 82 + (6 * 5)},
                                     {31, 82 + (6 * 6), -1, false},
                                 }},
                                 {{
                                     {36, 0}, // LIL PABLO
                                     {41, 1},
                                     {42, 6},
                                     {43, 12},
                                     {44, 12 + (6 * 1)},
                                     {45, 12 + (6 * 2)},
                                     {40, 12 + (6 * 3)},
                                     {41, 12 + (6 * 4)},
                                     {42, 12 + (6 * 5)},
                                     {43, 12 + (6 * 6)},
                                     {44, 12 + (6 * 7)},
                                     {45, 12 + (6 * 8)},
                                     {19, 12 + (6 * 9), 3, true, 5},
                                     {20, 12 + (6 * 10)},
                                     {41, 12 + (6 * 11), -2},
                                     {14, 12 + (6 * 12), 4, true, 5},
                                     {15, 12 + (6 * 13)},
                                     {44, 12 + (6 * 14), -2},
                                     {19, 12 + (6 * 15), 4, true, 5},
                                     {20, 12 + (6 * 16)},
                                     {41, 12 + (6 * 17), -2},
                                     {14, 12 + (6 * 18), 4, true, 5},
                                     {15, 12 + (6 * 19)},
                                     {23, 12 + (6 * 20)},
                                     {24, 12 + (6 * 21)},
                                     {25, 12 + (6 * 22)},
                                     {26, 12 + (6 * 23)},
                                     {27, 12 + (6 * 24), 20, true, 5},
                                     {28, 12 + (6 * 25)},
                                     {29, 12 + (6 * 26)},
                                     {30, 12 + (6 * 27), -1, false},
                                 }},
                                 {{
                                     {43, 0}, // SVETLANA
                                     {44, 1},
                                     {45, 6},
                                     {46, 18},
                                     {0, 36},
                                     {28, 36 + (6 * 1), -2},
                                     {29, 36 + (6 * 2)},
                                     {30, 36 + (6 * 3)},
                                     {31, 36 + (6 * 4)},
                                     {32, 36 + (6 * 5)},
                                     {33, 36 + (6 * 6), 3, true, 5},
                                     {34, 36 + (6 * 7)},
                                     {35, 36 + (6 * 8)},
                                     {28, 36 + (6 * 9), -2},
                                     {17, 36 + (6 * 10)},
                                     {18, 36 + (6 * 11)},
                                     {19, 36 + (6 * 12), 4, true, 5},
                                     {20, 36 + (6 * 13)},
                                     {30, 36 + (6 * 14), -2},
                                     {31, 36 + (6 * 15)},
                                     {32, 36 + (6 * 16)},
                                     {33, 36 + (6 * 17), 3, true, 5},
                                     {34, 36 + (6 * 18)},
                                     {35, 36 + (6 * 19)},
                                     {28, 36 + (6 * 20), -2},
                                     {17, 36 + (6 * 21)},
                                     {18, 36 + (6 * 22)},
                                     {19, 36 + (6 * 23), 4, true, 5},
                                     {20, 36 + (6 * 24)},
                                     {22, 36 + (6 * 25), -2},
                                     {23, 36 + (6 * 27)},
                                     {24, 36 + (6 * 29)},
                                     {33, 36 + (6 * 31), 20, true, 5},
                                     {34, 36 + (6 * 32)},
                                     {35, 36 + (6 * 33)},
                                     {36, 36 + (6 * 34)},
                                     {37, 36 + (6 * 35), -1, false},
                                 }},
                                 {{{62, 0}, // KAIRO
                                   {63, 1},
                                   {64, 6},
                                   {65, 12},
                                   {47, 18},
                                   {48, 18 + (12 * 1)},
                                   {89, 18 + (12 * 2)},
                                   {90, 18 + (12 * 3)},
                                   {47, 65 + (7 * 1)},
                                   {48, 65 + (7 * 2)},
                                   {49, 65 + (7 * 3)},
                                   {50, 65 + (7 * 4)},
                                   {51, 65 + (7 * 5)},
                                   {52, 65 + (7 * 6), 35, true, 7},
                                   {53, 65 + (7 * 7)},
                                   {54, 65 + (7 * 8)},
                                   {55, 65 + (7 * 9)},
                                   {56, 65 + (7 * 10), -1, false}}}};

static traits* GetStatsForBoxer(Character character)
{
  if (int(character) < 6 && int(character) >= 0)
  {
    return &chars[character];
  }
  else
  {
    return nullptr;
  }
};

static superSequence* GetSuperSequence(Character character)
{
  if (int(character) < 6 && int(character) >= 0)
  {
    return &super[character];
  }
  else
  {
    return nullptr;
  }
};