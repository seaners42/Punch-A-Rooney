#include "game.h"
#include "nds/arm9/input.h"
#include "nds/input.h"
#include "nds/interrupts.h"
#include "nds/system.h"
#include <sys/unistd.h>

// PUNCH-A-ROONEY!
// Sean David

// This project was actually an overhaul of a PyGame project I made in about 5 days.
// This two-week project took two months before it was finally released onto my GitHub
// I've learned a lot making this game, so this code may not be the most up-to-par with
// most coding standards or even game-dev code organization

// But the game is pretty fun regardless!

int main(int argc, char** argv)
{
  Game game;

  game.initializeDS(argc, argv);

  game.setup2DGraphics(false);
  game.startIntro();

  while (game.running())
  {
    game.handleEvents();
    game.update();
    game.render();
  }

  return 0;
}