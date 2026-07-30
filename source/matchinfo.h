#pragma once

#include <maxmod9.h>

#include "enums.h"
#include "soundbank.h"

struct Match
{
  const Character storyOpponent;

  const mm_word storyMusic;
  const int difficulty;

  const char preview[20];
  const char previewbg[14];

  const char quotetext[3][20];
  const int quotelines;
};

inline mm_word stageMusic[12] = {
    MOD_SLIMJIM, MOD_SLIMJIM, MOD_JILL,  MOD_PABLO,    MOD_SVETLANA, MOD_SLIMJIM,
    MOD_SLIMJIM, MOD_JILL,    MOD_PABLO, MOD_SVETLANA, MOD_SLIMJIM,
};

static Match Matches[11]{
    {ROONEY, MOD_SLIMJIM, 5, "bg/Preview1", "bg/PreviewBg1", {"Yahoo!", "Wahoo!"}, 2},
    {SLIMJIM,
     MOD_SLIMJIM,
     5,
     "bg/Preview1",
     "bg/PreviewBg1",
     {"I'll try to take it", "easy on you baldy!"},
     2},
    {JILL,
     MOD_JILL,
     10,
     "bg/Preview2",
     "bg/PreviewBg2",
     {"If I go down, I go", "down fighting, mew."},
     2},
    {LILPABLO,
     MOD_PABLO,
     15,
     "bg/Preview3",
     "bg/PreviewBg3",
     {"Para mi familia,", "you won't stop me."},
     2},
    {SVETLANA,
     MOD_SVETLANA,
     350,
     "bg/Preview4",
     "bg/PreviewBg4",
     {"Don't be so shy,", "Podaydi!"},
     2},
    {KAIRO,
     MOD_KAIRO,
     450,
     "bg/Preview5",
     "bg/PreviewBg5",
     {"Alright hot shot,", "come and face me."},
     2},

    // Title Defense Matches
    {SLIMJIM,
     MOD_SLIMJIM,
     50,
     "bg/Preview1Champ",
     "bg/PreviewBg1",
     {"Okay, that first", "fight don't count!"},
     2},
    {JILL,
     MOD_JILL,
     75,
     "bg/Preview2Champ",
     "bg/PreviewBg2",
     {"This time, you'll", "topple like a", "cat tree."},
     3},
    {LILPABLO,
     MOD_PABLO,
     100,
     "bg/Preview3Champ",
     "bg/PreviewBg3",
     {"I'll make sure this", "fight ain't easy", "for you, senor."},
     3},
    {SVETLANA,
     MOD_SVETLANA,
     500,
     "bg/Preview4Champ",
     "bg/PreviewBg4",
     {"No more messing", "around with you."},
     2},
    {KAIRO,
     MOD_KAIRO,
     1000,
     "bg/Preview5Champ",
     "bg/PreviewBg5",
     {"Like being champ?", "enjoy it while", "it lasts."},
     3},

};

static Match* GetMatch(int match)
{
  if (match > 0 && match < 11)
    return &Matches[match];
  else
    return nullptr;
}