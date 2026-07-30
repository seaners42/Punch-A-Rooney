#include "game.h"
#include "nds/arm9/input.h"
#include "nds/input.h"
#include "nds/interrupts.h"
#include "nds/system.h"
#include <sys/unistd.h>

// PUNCH-A-ROONEY!

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