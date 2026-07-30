#include "referee.h"
#include "enums.h"
#include "menu.h"

// Construct boxer using provided arguments
Referee::Referee(int index) : m_index(index) {}

Referee::~Referee() {}

void Referee::unload()
{
  NF_DeleteSprite(0, m_index);

  NF_FreeSpriteGfx(0, m_index);
  NF_UnloadSpriteGfx(m_index);
  NF_UnloadSpritePal(m_index);

  mmUnloadEffect(SFX_REF_FIGHT);
  mmUnloadEffect(SFX_REF_STOP);
  mmUnloadEffect(SFX_REF_ITSOVER);
  mmUnloadEffect(SFX_REF_WINNER);
  for (int sfx : count)
  {
    mmUnloadEffect(sfx);
  }
}

// Create the actual sprite
int Referee::load()
{
  NF_LoadSpriteGfx("sprite/referee", m_index, 64, 64);
  NF_LoadSpritePal("sprite/referee", m_index);

  // Allocate from ram to vram
  NF_VramSpriteGfx(0, m_index, m_index, true);
  NF_VramSpritePal(0, m_index, m_index);

  // Create the sprite
  NF_CreateSprite(0, m_index, m_index, m_index, 0, 0);
  NF_EnableSpriteRotScale(0, m_index, m_index, true);

  m_x = 96;
  askedToBox = false;

  mmLoadEffect(SFX_REF_FIGHT);
  mmLoadEffect(SFX_REF_STOP);
  mmLoadEffect(SFX_REF_ITSOVER);
  mmLoadEffect(SFX_REF_WINNER);

  for (int sfx : count)
  {
    mmLoadEffect(sfx);
  }

  return 0;
};

void Referee::hide(bool show)
{
  show ? NF_EnableSpriteRotScale(0, m_index, m_index, true) : NF_DisableSpriteRotScale(0, m_index);
  NF_ShowSprite(0, m_index, show);
}

void Referee::resetPosition()
{
  m_x = 96;
  m_state = REFIDLE;
  askedToBox = false;
}

void Referee::movementHandler(Boxer& boxer1, Boxer& boxer2, bool roundActive, bool matchActive)
{
  if (roundActive && !askedToBox)
  {
    m_state = BOX;
    askedToBox = true;
  }

  if (!roundActive && askedToBox && matchActive)
  {
    m_state = ROUNDEND;
  }

  if (tick % 30 == 0)
  {
    if (std::abs((m_x - 8) - midpoint) > 25 && roundActive && m_state == REFIDLE)
    {
      m_movement = ((m_x - 8) - midpoint > 0 ? LEFT : RIGHT);
    }

    else
    {
      m_movement = STILL;
    }
  }

  if (roundActive)
    m_looking = ((m_x - 8) - midpoint > 0 ? LEFT : RIGHT);

  if ((boxer1.m_state == DOWN || boxer2.m_state == DOWN) && timeSpentCounting < 590 &&
      timeSpentCounting >= -40)
  {
    if (timeSpentCounting >= 0 && matchActive)
    {
      m_state = COUNT;
    }
    m_movement = STILL;
    timeSpentCounting++;

    int whereToLook = boxer1.m_state == DOWN ? boxer1.m_x : boxer2.m_x;

    m_looking = ((m_x - 8) - whereToLook > 0 ? LEFT : RIGHT);

    if (timeSpentCounting % 60 == 18 && (matchActive || timeSpentCounting == 558))
    {
      mmEffect(count[((timeSpentCounting) / 60)]);
    }

    if (timeSpentCounting % 60 == 0 && matchActive)
    {
      m_intClock = 0;
    }
  }
  else
  {
    timeSpentCounting = matchActive ? -40 : -100;
  }

  switch (m_movement)
  {
    case LEFT:
      m_dx = std::max(m_dx - 1, -1);
      break;
    case RIGHT:
      m_dx = std::min(m_dx + 1, 1);
      break;
    case STILL:
      if (m_dx > 0)
        m_dx = std::max(m_dx - 1, 0);
      else
        m_dx = std::min(m_dx + 1, 0);
      break;
      if (m_dy > 0)
        m_dy = std::max(m_dy - 1, 0);
      else
        m_dy = std::min(m_dy + 1, 0);
      break;
  }

  if (m_x == 32 || m_x == 160)
  {
    m_movement = STILL;
  }

  if (tick % 3 == 0)
    m_y = std::clamp(m_y + m_dy, 90, 95);
  m_x = std::clamp(m_x + m_dx, 32, 160);
}

void Referee::animationHandler(Boxer& boxer1, Boxer& boxer2, bool matchActive)
{
  bool backwards = (false);

  int frames[2] = {matchActive ? (m_movement == STILL ? 0 : 4) : 16,
                   matchActive ? (m_movement == STILL ? 3 : 7) : 19};

  switch (m_state)
  {
    case COUNT:
      if (m_intClock == 0)
      {
        m_frame = 12;
      }
      if (m_intClock % 6 == 5)
      {
        if (m_frame < 15)
          m_frame++;
        else
        {
          if (timeSpentCounting == -40)
          {
            m_state = BOX;
            allowMovement = false;
            m_intClock = 0;
            m_indieClock = 0;
          }
          break;
        }
      }
      m_intClock++;
      break;
    case BOX:
      if (m_intClock == 0)
      {
        m_frame = 8;
      }

      if (m_intClock == 18)
      {
        mmEffect(SFX_REF_FIGHT);
      }

      if (m_intClock % 6 == 5)
      {
        if (m_frame < 11)
          m_frame++;
      }
      if (m_intClock == 40)
      {
        allowMovement = true;
      }
      if (m_intClock == 60)
      {
        m_state = REFIDLE;
        m_intClock = 0;
        m_indieClock = 0;
        break;
      }
      m_intClock++;
      break;
    case DECISION:
      if (m_intClock == 0)
      {
        m_frame = 24;
      }
      if (m_intClock % 6 == 5)
      {
        if (m_frame < 27)
          m_frame++;
        else
        {
          break;
        }
      }
      m_intClock++;
      break;
    case ROUNDEND:
      if (m_intClock == 0)
      {
        m_frame = 20;
      }

      if (m_intClock == 18)
      {
        mmEffect(SFX_REF_STOP);
      }

      if (m_intClock % 6 == 5)
      {
        if (m_frame < 23)
          m_frame++;
        else
          break;
      }
      m_intClock++;
      break;

    default:
      m_state = REFIDLE;

      if (m_indieClock == 0)
      {
        m_frame = frames[0];
      }

      if (m_frame > frames[1] || m_frame < frames[0])
      {
        m_frame = frames[0];
        m_indieClock = 0;
      }

      if (m_indieClock % 12 == 11)
      {
        if (m_frame == (m_movement == backwards ? frames[0] : frames[1]))
        {
          m_frame = m_movement == backwards ? frames[1] : frames[0];
        }
        else
          m_movement == backwards ? m_frame-- : m_frame++;
      }

      if (makingDecision)
        m_frame = 24;

      m_intClock = 0;
      m_indieClock++;
      break;
  }
}

void Referee::decisionSequence(int expectedWinner, bool onlyFirstFrame)
{
  makingDecision = true;
  m_looking = LEFT;

  if (!onlyFirstFrame)
  {
    m_state = DECISION;
    m_looking = expectedWinner == 1 ? RIGHT : LEFT;
  }
}

void Referee::cameraHandler(int& screenX, int& screenY)
{
  screenX = int(((m_x - fOffsetX) * fScale) - (32 / fScale));
  screenY = int(((m_y - fOffsetY) * fScale) - (32 / fScale));

  NF_SpriteRotScale(0, m_index, 0, (m_looking == LEFT ? 1 : -1) * int(256 * fScale),
                    int(256 * fScale));
}

void Referee::update(Boxer& boxer1, Boxer& boxer2, bool roundActive, bool matchActive)
{
  midpoint = (boxer1.m_x + boxer2.m_x) / 2;

  animationHandler(boxer1, boxer2, matchActive);
  movementHandler(boxer1, boxer2, roundActive, matchActive);

  int screen_x;
  int screen_y;

  cameraHandler(screen_x, screen_y);

  NF_MoveSprite(0, m_index, screen_x, screen_y);
  NF_SpriteFrame(0, m_index, m_frame);

  m_dy = !matchActive || !roundActive || (m_state != REFIDLE) ? 1 : -1;
}
