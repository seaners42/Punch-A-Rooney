#include "save.h"

bool load_data(const char* path)
{
  FILE* f = fopen(path, "rb");
  if (f == NULL)
  {
    printf("Can't open file\n");
    return false;
  }

  if (fread(&game_data, sizeof(game_data), 1, f) != 1)
  {
    printf("Can't read file\n");
    return false;
  }

  if (fclose(f) != 0)
  {
    printf("Can't close file\n");
    return false;
  }

  return true;
}

bool save_data(const char* path)
{
  FILE* f = fopen(path, "wb");
  if (f == NULL)
  {
    printf("Can't open file\n");
    return false;
  }

  if (fwrite(&game_data, sizeof(game_data), 1, f) != 1)
  {
    printf("Can't write file\n");
    return false;
  }

  if (fclose(f) != 0)
  {
    printf("Can't close file\n");
    return false;
  }

  return true;
}

void reset_data()
{
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
}