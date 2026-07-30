#include "boxer.h"
#include "enums.h"
#include "menu.h"
#include "nds/system.h"

// Construct boxer using provided arguments
Boxer::Boxer(Character character, int npc, int index, bool isPlayerOne, bool titleDefense)
    : m_character(character), m_isPlayerOne(isPlayerOne), m_aiLevel(npc),
      m_titleDefense(titleDefense), m_index(index)
{
}

Boxer::~Boxer() {}

void Boxer::unload()
{
  NF_DeleteSprite(0, m_index);
  NF_DeleteSprite(0, m_index + 2);

  NF_FreeSpriteGfx(0, m_index);
  NF_UnloadSpriteGfx(m_index);
  NF_UnloadSpritePal(m_index);

  NF_FreeSpriteGfx(0, m_index + 2);
  NF_UnloadSpriteGfx(m_index + 2);
  NF_UnloadSpritePal(m_index + 2);

  mmUnloadEffect(SFX_LIGHT);
  mmUnloadEffect(SFX_LIGHTMISS);
  mmUnloadEffect(SFX_WEAK);
  mmUnloadEffect(SFX_VIOLA_GET);

  switch (m_character)
  {
    case ROONEY:
      mmUnloadEffect(SFX_RN_HURT1);
      mmUnloadEffect(SFX_RN_HURT2);
      mmUnloadEffect(SFX_RN_WIN);
      mmUnloadEffect(SFX_RN_DOWN);
      mmUnloadEffect(SFX_RN_SUPER1);
      mmUnloadEffect(SFX_RN_SUPER2);
      mmUnloadEffect(SFX_RN_SUPER3);
      break;
    case SLIMJIM:
      mmUnloadEffect(SFX_SJ_HURT1);
      mmUnloadEffect(SFX_SJ_HURT2);
      mmUnloadEffect(SFX_SJ_WIN);
      mmUnloadEffect(SFX_SJ_DOWN);
      mmUnloadEffect(SFX_SJ_SUPER);
      break;
    case JILL:
      mmUnloadEffect(SFX_JL_HURT1);
      mmUnloadEffect(SFX_JL_HURT2);
      mmUnloadEffect(SFX_JL_DOWN);
      mmUnloadEffect(SFX_JL_WIN);
      break;
    case LILPABLO:
      mmUnloadEffect(SFX_PB_HURT1);
      mmUnloadEffect(SFX_PB_HURT2);
      mmUnloadEffect(SFX_PB_DOWN);
      mmUnloadEffect(SFX_PB_WIN);
      break;
    case SVETLANA:
      mmUnloadEffect(SFX_LN_HURT1);
      mmUnloadEffect(SFX_LN_HURT2);
      mmUnloadEffect(SFX_LN_DOWN);
      mmUnloadEffect(SFX_LN_WIN);
      break;
    case KAIRO:
      mmUnloadEffect(SFX_KR_HURT1);
      mmUnloadEffect(SFX_KR_HURT2);
      mmUnloadEffect(SFX_KR_KO);
      mmUnloadEffect(SFX_KR_DOWN);
      break;
  }

  m_traits = nullptr;
  m_superSeq = nullptr;
}

// Create the actual sprite
int Boxer::load()
{
  m_traits = GetStatsForBoxer(m_character);
  m_superSeq = GetSuperSequence(m_character);

  // Uses a Character Enum to choose a gfx and pal
  NF_LoadSpriteGfx(boxerPaths[m_character + (m_titleDefense ? 6 : 0)], m_index, 64, 64);
  NF_LoadSpritePal(boxerPaths[m_character + (m_titleDefense ? 6 : 0)], m_index);

  NF_LoadSpriteGfx("sprite/blockbar", m_index + 2, 32, 32);
  NF_LoadSpritePal("sprite/blockbar", m_index + 2);

  // Allocate from ram to vram
  NF_VramSpriteGfx(0, m_index, m_index, true);
  NF_VramSpritePal(0, m_index, m_index);

  NF_VramSpriteGfx(0, m_index + 2, m_index + 2, true);
  NF_VramSpritePal(0, m_index + 2, m_index + 2);

  // Create the sprite
  NF_CreateSprite(0, m_index, m_index, m_index, 0, 0);
  NF_EnableSpriteRotScale(0, m_index, m_index, true);

  NF_CreateSprite(0, m_index + 2, m_index + 2, m_index + 2, 0, 0);
  NF_EnableSpriteRotScale(0, m_index + 2, m_index + 2, true);

  m_x = m_isPlayerOne ? 0 : SCREEN_WIDTH - 64;

  if (m_character == KAIRO)
  {
    m_kairoHavingFun = true;
  }

  m_punchesLanded = 0;
  m_punchesThrown = 0;

  jukedPunchFreq = m_titleDefense ? 12 : 18;

  mmLoadEffect(SFX_LIGHT);
  mmLoadEffect(SFX_LIGHTMISS);
  mmLoadEffect(SFX_WEAK);
  mmLoadEffect(SFX_VIOLA_GET);

  switch (m_character)
  {
    case ROONEY:
      mmLoadEffect(SFX_RN_HURT1);
      mmLoadEffect(SFX_RN_HURT2);
      mmLoadEffect(SFX_RN_WIN);
      mmLoadEffect(SFX_RN_DOWN);
      mmLoadEffect(SFX_RN_SUPER1);
      mmLoadEffect(SFX_RN_SUPER2);
      mmLoadEffect(SFX_RN_SUPER3);

      sfxhurt1 = SFX_RN_HURT1;
      sfxhurt2 = SFX_RN_HURT2;
      sfxdown = SFX_RN_DOWN;
      sfxwin = SFX_RN_WIN;

      break;
    case SLIMJIM:
      mmLoadEffect(SFX_SJ_HURT1);
      mmLoadEffect(SFX_SJ_HURT2);
      mmLoadEffect(SFX_SJ_WIN);
      mmLoadEffect(SFX_SJ_DOWN);
      mmLoadEffect(SFX_SJ_SUPER);

      sfxhurt1 = SFX_SJ_HURT1;
      sfxhurt2 = SFX_SJ_HURT2;
      sfxdown = SFX_SJ_DOWN;
      sfxwin = SFX_SJ_WIN;

      break;
    case JILL:
      mmLoadEffect(SFX_JL_HURT1);
      mmLoadEffect(SFX_JL_HURT2);
      mmLoadEffect(SFX_JL_DOWN);
      mmLoadEffect(SFX_JL_WIN);

      sfxhurt1 = SFX_JL_HURT1;
      sfxhurt2 = SFX_JL_HURT2;
      sfxdown = SFX_JL_DOWN;
      sfxwin = SFX_JL_WIN;
      break;
    case LILPABLO:
      mmLoadEffect(SFX_PB_HURT1);
      mmLoadEffect(SFX_PB_HURT2);
      mmLoadEffect(SFX_PB_DOWN);
      mmLoadEffect(SFX_PB_WIN);

      sfxhurt1 = SFX_PB_HURT1;
      sfxhurt2 = SFX_PB_HURT2;
      sfxdown = SFX_PB_DOWN;
      sfxwin = SFX_PB_WIN;
      break;
    case SVETLANA:
      mmLoadEffect(SFX_LN_HURT1);
      mmLoadEffect(SFX_LN_HURT2);
      mmLoadEffect(SFX_LN_DOWN);
      mmLoadEffect(SFX_LN_WIN);

      sfxhurt1 = SFX_LN_HURT1;
      sfxhurt2 = SFX_LN_HURT2;
      sfxdown = SFX_LN_DOWN;
      sfxwin = SFX_LN_WIN;
      break;
    case KAIRO:
      mmLoadEffect(SFX_KR_HURT1);
      mmLoadEffect(SFX_KR_HURT2);
      mmLoadEffect(SFX_KR_KO);
      mmLoadEffect(SFX_KR_DOWN);

      sfxhurt1 = SFX_KR_HURT1;
      sfxhurt2 = SFX_KR_HURT2;
      sfxdown = SFX_KR_DOWN;
      sfxwin = SFX_KR_KO;

      break;
  }

  return 0;
};

void Boxer::hide(bool show)
{
  show ? NF_EnableSpriteRotScale(0, m_index, m_index, true) : NF_DisableSpriteRotScale(0, m_index);
  NF_ShowSprite(0, m_index, show);

  show ? NF_EnableSpriteRotScale(0, m_index + 2, m_index + 2, true)
       : NF_DisableSpriteRotScale(0, m_index + 2);
  NF_ShowSprite(0, m_index + 2, show);
}

void Boxer::resetPosition()
{
  m_x = m_isPlayerOne ? 0 : SCREEN_WIDTH - 64;
  m_state = IDLE;
  m_tireouts = 0;
  m_rounddowns = 0;
}

void Boxer::handleInput()
{
  if (m_aiLevel == 0 && !m_superActive)
  {
    if (m_state != TIRED && !m_boxerLocked)
    {
      if (keysDown() && m_state == LANATAUNT)
      {
        m_state = IDLE;
      }

      if (keysHeld() & KEY_LEFT && (m_state == IDLE || m_state == BLOCK))
        m_movement = LEFT;
      else if (keysHeld() & KEY_RIGHT && (m_state == IDLE || m_state == BLOCK))
        m_movement = RIGHT;
      else
        m_movement = STILL;

      if (keysHeld() & KEY_A && m_state == IDLE && m_canPunch && m_stamina >= 0)
      {
        m_state = LIGHT;
        m_stamina = m_stamina - 10;
      }
      if (keysHeld() & KEY_B && m_state == IDLE && m_canPunch && m_stamina >= 0)
      {
        m_state = HEAVY;
        m_stamina = m_stamina - 15;
      }
      if (keysHeld() & KEY_X && m_state == IDLE && m_stamina > 0 && m_super >= 250)
      {
        m_state = SUPER;
        m_stamina = std::max(m_stamina - 25, 0);
        m_super = 0;
      }
      if (keysDown() & KEY_Y && m_state == IDLE && m_character == SVETLANA)
      {
        m_state = LANATAUNT;
      }

      if (keysDown() & KEY_R && (m_state == IDLE || m_state == BLOCK) && m_stamina > 0 &&
          m_canUnblock)
      {
        m_state = DODGE;
        m_stamina = std::max(m_stamina - 10, 0);
      }
      if (keysHeld() & KEY_L && m_state == IDLE && m_canBlock && m_block > 0)
        m_state = BLOCK;
      if (!(keysHeld() & KEY_L) && m_state == BLOCK && m_canUnblock)
        m_state = IDLE;

      if (m_kairoHavingFun)
      {
        switch (m_state)
        {
          case LIGHT:
            m_state = TAP;
            m_stamina += 5;
            break;
          case HEAVY:
            m_state = PUSH;
            m_stamina += 5;
            break;
          default:
            break;
        }
      }
    }
    else
      m_movement = STILL;

    if (keysDown() & KEY_TOUCH && m_state == DOWN)
      downHandler(true);
  }
}

void Boxer::unlock()
{
  m_boxerLocked = false;
  if (m_state != DOWN)
  {
    m_state = IDLE;
    m_intClock = 0;
    m_indieClock = 0;
  }
}

void Boxer::damage(int dmg, int knock, bool superLock, bool hurts)
{
  int sfx;
  int punchvol = (dmg > 10 ? 600 : 1024);
  int hurtsfx;
  m_damagedAlready = true;

  if (m_state != DOWN)
  {
    if (m_state == BLOCK || m_state == BLOCK_SCRIPT)
    {
      m_block = m_block - dmg;
      m_shieldWait = tick;
      m_canUnblock = false;
      NF_SpriteSetPalColor(0, m_index + 2, 1, 31, 31, 31);
      NF_SpriteSetPalColor(0, m_index + 2, 3, 31, 31, 31);
      sfx = SFX_WEAK;
      if (m_block <= 0)
      {
        if (m_aiLevel != -1)
          m_hp = m_hp - (int(dmg * 1.2));
        if (hurts)
          m_state = DMG;
        m_intClock = 0;
        m_lastDamaged = tick;
        m_boxerLocked = superLock;
        sfx = SFX_LIGHT;
      }
    }
    else
    {
      if (m_aiLevel != -1)
        m_hp = m_hp - ((m_state == HEAVY || m_state == LIGHT) ? int(dmg * 1.2) : dmg);
      if (hurts)
        m_state = DMG;
      m_intClock = 0;
      m_lastDamaged = tick;
      m_boxerLocked = superLock;
      sfx = SFX_LIGHT;
    }

    if (!m_isPlayerOne)
      m_dx = knock;
    else
      m_dx = -knock;

    if (m_hp <= 0)
    {
      m_state = DOWN;
      m_dx = m_isPlayerOne ? -10 : 10;
      m_downs++;
      m_rounddowns++;
      m_getUpCount = 0;
      sfx = SFX_LIGHT;
      punchvol = 350;
    }

    if (!hurts)
    {
      sfx = SFX_WEAK;
      m_state = IDLE;
      m_punchWait = tick + 27;
    }

    int range = 100 - -100 + 1;
    int num = rand() % range + -50;

    mmEffectRate(mmEffect(sfx), punchvol + num);
    if (hurts && sfx == SFX_LIGHT)
    {
      hurtsfx = m_state == DOWN ? sfxdown : num < 0 ? sfxhurt1 : sfxhurt2;
      mmEffectRate(mmEffect(hurtsfx), 1024);
    }
  }
}

void Boxer::downHandler(bool mp)
{
  {
    if (m_frame >= m_traits->downframes[1] || mp)
    {
      int punish = m_hp < 0 ? abs(m_hp) / 2 : 0;

      m_getUpCount += 20 - punish;
      m_rot = (m_getUpCount % 2) ? 3 : -3;

      if (m_getUpCount >= 30)
        m_frame = m_traits->getupframes[0];
      if (m_getUpCount >= 70)
        m_frame = m_traits->getupframes[1];

      if (m_getUpCount >= 100)
      {
        m_state = IDLE;
        m_rot = 0;
        m_intClock = 0;
        m_hp = 150 - std::min((m_downs * 37), 100);
        m_getUpCount = 0;
      }
    }
  }
}

void Boxer::movementHandler(Boxer& opponent, bool roundActive, bool matchActive)
{
  Movement backwards = m_isPlayerOne ? LEFT : RIGHT;

  if (opponent.m_state == DOWN && m_state != SUPERSEQ && roundActive)
  {
    m_movement = backwards;
  }

  if (!roundActive && m_state != DOWN)
  {
    m_movement = (!matchActive && m_state == IDLE) ? backwards : STILL;
  }

  if (m_state == DOWN)
  {
    m_movement = STILL;
  }

  m_y = m_y + m_dy;
  m_x = m_isPlayerOne ? std::clamp(m_x + m_dx, -18, opponent.m_x)
                      : std::clamp(m_x + m_dx, opponent.m_x, SCREEN_WIDTH - 64 + 18);

  switch (m_movement)
  {
    case LEFT:
      m_dx = std::max(m_dx - 1, -m_maxSpeed);
      break;
    case RIGHT:
      m_dx = std::min(m_dx + 1, m_maxSpeed);
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

  if (m_x == (m_isPlayerOne ? -18 : SCREEN_WIDTH - 64 + 18) || m_x == opponent.m_x)
  {
    m_movement = STILL;
  }
}

void Boxer::npcHandler(int lvl, Boxer& opponent)
{
  if (opponent.m_state != DOWN && m_state != TIRED && !m_boxerLocked)
  {
    if (!m_superActive)
    {
      bool inRange = abs(m_x - opponent.m_x) < m_traits->punchdist;
      bool initiate = opponent.m_juked || (opponent.m_state == TIRED);
      bool svetlanaTaunt = (m_state == LANATAUNT && opponent.m_intClock == 12);
      bool svetlanaTauntHeavy = (m_state == LANATAUNT && opponent.m_intClock == 27);

      // Cower if low on essential stats
      if ((m_stamina < 15 || m_block < 10) && (m_state == IDLE || m_state == BLOCK_SCRIPT))
        m_movement = m_isPlayerOne ? LEFT : RIGHT;

      // Move towards opponent if too far away
      else if (!inRange && (m_state == IDLE || m_state == BLOCK_SCRIPT))
        m_movement = m_isPlayerOne ? RIGHT : LEFT;

      else
        m_movement = STILL;

      if (m_state == DOWN && (tick % std::max(13 - (lvl / 5), 6) == 0) && rand() % 10 > 3)
        downHandler(false);

      if (!inRange && m_state == IDLE && (rand() % 650 < lvl) && m_character == SVETLANA &&
          m_hp > 100 && opponent.m_hp > 100 && opponent.m_stamina > 50)
      {
        m_state = LANATAUNT;
      }

      // if opponent is tired or juked, take advantage
      // This takes higher precedent in the function because realistically most boxers would
      // prioritize initiative over defense

      if (inRange && initiate && (m_state == IDLE || m_state == BLOCK_SCRIPT))
      {
        if ((rand() % 20 < lvl) && m_canPunch && m_stamina > 10 && m_super >= 250)
        {
          m_intClock = 0;
          m_state = SUPER;
          m_stamina = std::max(m_stamina - 25, 0);
          m_super = 0;
        }
        else if ((rand() % 3 == 1) && m_canPunch && m_stamina > 10)
        {
          m_intClock = 0;
          m_state = LIGHT;
          m_stamina = m_stamina - 10;
        }
        else if (m_canPunch && m_stamina > 10)
        {
          m_intClock = 0;
          m_state = HEAVY;
          m_stamina = m_stamina - 15;
        }
      }

      // If other player is LIGHT attacking, you can block within a certain
      // window
      if ((opponent.m_state == LIGHT || opponent.m_state == TAP || opponent.m_state == PUSH) &&
          opponent.m_intClock < 15)
      {
        if ((rand() % 30) < lvl && m_canBlock && m_block > 0 && m_state == IDLE)
        {
          m_state = BLOCK_SCRIPT;
        }
        else if ((((rand() % 500) < lvl) && m_stamina > 0 && m_canUnblock &&
                  (m_state == IDLE || m_state == BLOCK_SCRIPT)) ||
                 svetlanaTaunt)
        {
          m_intClock = 0;
          m_state = DODGE;
          m_stamina = std::max(m_stamina - 10, 0);
        }
      }

      // If other player is HEAVY attacking, you can block or light counter
      // within a certain window

      if ((opponent.m_state == HEAVY) && opponent.m_intClock < 30)
      {
        if ((rand() % 60) < lvl && m_canBlock && m_block > 0 && m_state == IDLE)
        {
          m_state = BLOCK_SCRIPT;
        }
        else if (((rand() % 120) < lvl) && m_canPunch && m_stamina > 10 && m_state == IDLE)
        {
          m_intClock = 0;
          m_state = LIGHT;
          m_stamina = m_stamina - 10;
        }
        else if ((((rand() % 500) < lvl) && m_stamina > 0 && m_canUnblock &&
                  (m_state == IDLE || m_state == BLOCK_SCRIPT)) ||
                 svetlanaTauntHeavy)
        {
          m_intClock = 0;
          m_state = DODGE;
          m_stamina = std::max(m_stamina - 10, 0);
        }
      }

      // Handle offensive actions
      // Throw a punch if the other player isn't attacking

      if (opponent.m_state != LIGHT && m_state == IDLE && inRange)
      {
        if (((rand() % 120) < std::min(lvl, 75)) && m_canPunch && m_stamina > 10)
        {
          m_intClock = 0;
          m_state = LIGHT;
          m_stamina = m_stamina - 10;
        }
        else if (((rand() % 180) < std::min(lvl, 100)) && m_canPunch && m_stamina > 10)
        {
          m_intClock = 0;
          m_state = HEAVY;
          m_stamina = m_stamina - 15;
        }
        else if (((rand() % 1000) < std::min(lvl, 950)) && m_canPunch && m_stamina > 10 &&
                 m_canUnblock)
        {
          m_intClock = 0;
          m_state = DODGE;
          m_stamina = std::max(m_stamina - 10, 0);
        }
        else
          m_state = IDLE;
      }

      if (m_kairoHavingFun)
      {
        switch (m_state)
        {
          case LIGHT:
            m_state = TAP;
            m_stamina += 5;
            break;
          case HEAVY:
            m_state = PUSH;
            m_stamina += 5;
            break;
          default:
            break;
        }
      }
    }
  }
  else
    m_movement = STILL;
}

void Boxer::animationHandler(bool matchActive, Boxer& opponent)
{
  Movement backwards = (m_isPlayerOne ? LEFT : RIGHT);

  bool doCelly = !matchActive || (m_character == KAIRO && opponent.m_state == DOWN);

  int standingFrames[2] = {!doCelly ? (m_traits->idleframes[0] - (m_kairoHavingFun ? 4 : 0))
                                    : (m_traits->cellyframes[0]),
                           !doCelly ? (m_traits->idleframes[1] - (m_kairoHavingFun ? 4 : 0))
                                    : (m_traits->cellyframes[1])};

  int frames[2] = {m_movement == STILL ? (standingFrames[0])
                                       : (m_traits->walkframes[0] - (m_kairoHavingFun ? 4 : 0)),
                   m_movement == STILL ? (standingFrames[1])
                                       : (m_traits->walkframes[1] - (m_kairoHavingFun ? 4 : 0))};

  int blkframes[2] = {m_movement == STILL ? (m_traits->blockframes[0] - (m_kairoHavingFun ? 4 : 0))
                                          : (m_traits->blockframes[0] - (m_kairoHavingFun ? 4 : 0)),
                      m_movement == STILL
                          ? (m_traits->blockframes[0] - (m_kairoHavingFun ? 4 : 0))
                          : (m_traits->blockframes[1] - (m_kairoHavingFun ? 4 : 0))};

  switch (m_state)
  {
    case LIGHT:
      if (m_intClock == 0)
      {
        m_frame = m_lightswitch ? m_traits->lightframes1[0] : m_traits->lightframes2[0];
        m_juked = false;
        m_punchedAlready = false;
      }

      if (m_intClock % m_punchFreq == m_punchFreq - 1)
      {
        if (m_frame < (m_lightswitch ? m_traits->lightframes1[1] : m_traits->lightframes2[1]))
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          m_lightswitch = !m_lightswitch;
          m_juked = false;
          m_punchedAlready = false;
          if (m_stamina <= 0)
          {
            m_tireouts++;
            m_state = TIRED;
          }
          if (m_comboWindow)
            m_punchFreq = m_traits->combospeed;
          break;
        }
      }
      m_intClock++;
      break;

    case HEAVY:
      if (m_intClock == 0)
      {
        m_frame = m_traits->heavyframes[0] +
                  ((opponent.m_juked ||
                    (m_comboWindow && (opponent.m_state == IDLE || opponent.m_state == DMG)))
                       ? 2
                       : 0);
        m_juked = false;
        m_punchedAlready = false;
      }
      if (m_intClock % m_punchFreq == m_punchFreq - 1)
      {
        if (m_frame < m_traits->heavyframes[1])
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          m_juked = false;
          m_punchedAlready = false;
          if (m_stamina <= 0)
          {
            m_tireouts++;
            m_state = TIRED;
          }
          if (m_comboWindow)
            m_punchFreq = m_traits->combospeed;
          break;
        }
      }
      m_intClock++;
      break;

    case TAP:
      if (m_intClock == 0)
      {
        m_frame = m_lightswitch ? 24 : 28;
        m_juked = false;
        m_punchedAlready = false;
      }

      if (m_intClock % m_punchFreq == m_punchFreq - 1)
      {
        if (m_frame < (m_lightswitch ? 27 : 31))
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          m_lightswitch = !m_lightswitch;
          m_juked = false;
          m_punchedAlready = false;
          break;
        }
      }
      m_intClock++;
      break;

    case PUSH:
      if (m_intClock == 0)
      {
        m_frame = 32;
        m_juked = false;
        m_punchedAlready = false;
      }

      if (m_intClock % m_punchFreq == m_punchFreq - 1)
      {
        if (m_frame < 36)
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          m_juked = false;
          m_punchedAlready = false;
          break;
        }
      }
      m_intClock++;
      break;

    case SUPER:
      if (m_intClock == 0)
      {
        m_frame = m_traits->specialframes[0] + (opponent.m_juked ? 1 : 0);
        m_juked = false;
        m_punchedAlready = false;
      }
      if (m_intClock % m_punchFreq == m_punchFreq - 1)
      {
        if (m_frame < m_traits->specialframes[1])
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          m_juked = false;
          m_punchedAlready = false;
          if (m_stamina <= 0)
          {
            m_tireouts++;
            m_state = TIRED;
          }

          if (m_comboWindow)
            m_punchFreq = m_traits->combospeed;
          break;
        }
      }
      m_intClock++;
      break;

    case DODGE:
      if (m_intClock == 0)
      {
        m_frame = m_traits->dodgeframes[0];
        m_dx += m_traits->dodgedistance * (m_isPlayerOne ? -1 : 1);
        m_blockWait = tick - 30;
      }
      if (m_intClock % m_traits->dodgespeed == m_traits->dodgespeed - 1)
      {
        if (m_frame < m_traits->dodgeframes[1])
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          break;
        }
      }
      m_intClock++;
      break;

    case BLOCK:

      if (m_indieClock == 0)
      {
        m_frame = blkframes[0];
      }

      if (m_frame > blkframes[1] || m_frame < blkframes[0])
      {
        m_frame = blkframes[0];
        m_indieClock = 0;
      }

      if (m_indieClock % 12 == 11)
      {
        if (m_frame == (m_movement == backwards ? blkframes[0] : blkframes[1]))
        {
          m_frame = m_movement == backwards ? blkframes[1] : blkframes[0];
        }
        else
          m_movement == backwards ? m_frame-- : m_frame++;
      }
      m_intClock = 0;
      m_indieClock++;
      break;
    case BLOCK_SCRIPT:
      if (m_indieClock == 0)
      {
        m_frame = blkframes[0];
      }

      if (m_frame > blkframes[1] || m_frame < blkframes[0])
      {
        m_frame = blkframes[0];
        m_indieClock = 0;
      }

      if (m_indieClock % m_traits->walkspeed == m_traits->walkspeed - 1)
      {
        if (m_frame == (m_movement == backwards ? blkframes[0] : blkframes[1]))
        {
          m_frame = m_movement == backwards ? blkframes[1] : blkframes[0];
        }
        else
          m_movement == backwards ? m_frame-- : m_frame++;
      }
      if (m_intClock > 45)
      {
        m_intClock = 0;
        m_indieClock = 0;
        m_state = IDLE;
        break;
      }

      m_intClock++;
      m_indieClock++;
      break;

    case DMG:
      if (m_intClock == 0)
      {
        m_frame = m_traits->dmgframes[0];
        m_juked = false;
        m_punchedAlready = false;
      }
      if (m_intClock % 6 == 5)
      {
        if (m_frame < m_traits->dmgframes[1])
          m_frame++;
        else
        {
          m_intClock = 0;
          m_indieClock = 0;
          m_state = IDLE;
          break;
        }
      }
      m_intClock++;
      break;

    case TIRED:
      if (m_intClock == 0)
      {
        m_frame = m_traits->tiredframes[0];
        m_blockWait = tick;
      }
      if (m_intClock % 12 == 11)
      {
        if (m_frame == m_traits->tiredframes[1])
        {
          m_frame = m_traits->tiredframes[0];
        }
        else
          m_frame++;
      }
      if ((m_intClock > 60 || !matchActive || opponent.m_state == DOWN) && !m_boxerLocked)
      {
        m_state = IDLE;
        m_intClock = 0;
        m_indieClock = 0;
        break;
      }
      m_intClock++;
      break;

    case LANATAUNT:
      if (m_intClock == 0)
      {
        m_frame = 12;
      }
      if (m_intClock % 12 == 11)
      {
        if (m_frame == 15)
        {
          m_frame = 12;
        }
        else
          m_frame++;
      }
      if ((m_intClock > 120 || !matchActive) && !m_boxerLocked)
      {
        m_state = IDLE;
        m_intClock = 0;
        m_indieClock = 0;

        if (m_aiLevel > 0)
        {
          int gotcha = rand() % 100;
          m_state = (gotcha < 40 ? HEAVY : LIGHT);
          m_stamina = m_stamina - 10;
        }

        break;
      }
      m_intClock++;
      break;

    case DOWN:
      if (m_intClock == 0)
      {
        m_frame = m_traits->downframes[0];
        m_juked = false;
        m_punchedAlready = false;
      }
      if (m_intClock % 6 == 5)
      {
        if (m_frame < m_traits->downframes[1])
          m_frame++;
        else
        {
          m_boxerLocked = false;
          break;
        }
      }
      m_intClock++;
      break;

    default:
      int walkspeed = m_kairoHavingFun ? 8 : m_traits->walkspeed;
      m_punchedAlready = false;

      if (m_boxerLocked)
      {
        m_state = TIRED;
        break;
      }
      m_state = IDLE;

      if (m_indieClock == 0)
      {
        m_frame = frames[0];
      }

      if (m_frame > frames[1] || m_frame < frames[0])
      {
        m_frame = frames[0];
        m_indieClock = 0;
      }

      if (m_indieClock % walkspeed == walkspeed - 1)
      {
        if (m_frame == (m_movement == backwards ? frames[0] : frames[1]))
        {
          m_frame = m_movement == backwards ? frames[1] : frames[0];
        }
        else
          m_movement == backwards ? m_frame-- : m_frame++;
      }
      m_intClock = 0;
      m_indieClock++;
      break;
  }
}

void Boxer::cameraHandler(int& screenX, int& screenY)
{
  screenX = int(((m_x - fOffsetX) * fScale) - (32 / fScale));
  screenY = int(((m_y - fOffsetY) * fScale) - (32 / fScale));

  int blockBarShake = 5 * exp(-0.2 * (tick - m_shieldWait)) * cos(tick - m_shieldWait);

  NF_MoveSprite(0, m_index + 2,
                int(((m_x + (m_isPlayerOne ? -34 : 62) - fOffsetX) * fScale) - (16 / fScale)),
                int((((m_y + 10 + blockBarShake) - fOffsetY) * fScale) - (16 / fScale)));

  NF_SpriteRotScale(0, m_index, m_rot, (m_isPlayerOne ? -1 : 1) * int(256 * fScale),
                    int(256 * fScale));
  NF_SpriteRotScale(0, m_index + 2, 0, int(128 * fScale), int(128 * fScale));
}

void Boxer::combatHandler(Boxer& opponent)
{
  m_canPunch = (tick - m_punchWait > 30) && !m_punchedAlready;
  m_canBlock = (tick - m_blockWait > 90);

  if (m_state == LIGHT || m_state == HEAVY)
    m_staminaWait = tick;
  if (m_state == TIRED || m_state == DODGE)
    m_staminaWait = tick + 30;

  m_kairoHavingFun = (m_character == KAIRO && (m_hp > 100 && opponent.m_hp > 100 &&
                                               opponent.m_stamina > 50 && opponent.m_block > 10));

  if (tick - m_staminaWait > 30 && m_stamina < 100)
  {
    m_stamina = std::max(m_stamina + 1, 0);
  }

  if (tick - m_shieldWait > 100 && m_block < 40)
  {
    if (tick % 16 == 0)
    {
      m_block = std::max(m_block + 1, 0);
    }
  }

  if (tick - m_shieldWait > 6)
  {
    NF_SpriteSetPalColor(0, m_index + 2, 1, 12, 31, 28);
    NF_SpriteSetPalColor(0, m_index + 2, 3, 9, 72, 15);
  }

  if (tick - m_shieldWait > 20)
  {
    m_canUnblock = true;
  }

  if (tick - m_comboWindow > 60 && m_punchFreq != m_traits->punchspeed)
  {
    m_punchFreq = ((!m_juked) ? (m_kairoHavingFun ? 8 : m_traits->punchspeed) : jukedPunchFreq) -
                  (m_titleDefense ? 1 : 0);
    m_comboWindow = 0;
  }

  if (!opponent.m_punchedAlready)
  {
    m_damagedAlready = false;
  }

  {
    if (opponent.confirmAction(opponent.m_state, *this, false))
    {
      opponent.hitPunch(opponent.m_state, *this, false, true);
    }
  }

  {
    if (confirmAction(m_state, opponent, true))
    {
      if (m_state == TAP || m_state == PUSH)
        m_comboWindow = 0;
      if (!hitPunch(m_state, opponent, false, false))
      {
        missedPunch(m_state, opponent, false);
      }
    }
  }
}

bool Boxer::hitPunch(Moves state, Boxer& opponent, bool flip, bool doDamage)
{
  bool inRange = abs(m_x - opponent.m_x) < m_traits->punchdist;
  int punchDamage =
      m_traits->heavydmg *
      ((m_comboWindow && (opponent.m_state == IDLE || opponent.m_state == DMG)) ? 0.7 : 1);

  bool hadSuper = m_super >= 250;

  if (inRange && opponent.m_state != DODGE)
  {
    switch (state)
    {
      case LIGHT:
        if (doDamage)
        {
          opponent.damage(m_traits->lightdmg, 5, false, true);
          break;
        }
        if (m_super < 250)
        {
          m_super += 10;
        }
        m_punchedAlready = true;
        m_comboWindow = tick;
        m_dx = (m_isPlayerOne ? 5 : -5);
        m_punchesLanded++;

        break;
      case HEAVY:
        if (doDamage)
        {
          opponent.damage(punchDamage, 7, false, true);
          break;
        }
        if (m_super < 250)
        {
          m_super += 20;
        }
        m_punchedAlready = true;
        m_comboWindow = tick;
        m_dx = (m_isPlayerOne ? 5 : -5);
        m_punchesLanded++;

        break;
      case SUPER:
        if (doDamage)
        {
          opponent.damage(15, 5, true, true);
          break;
        }
        m_punchedAlready = true;
        m_comboWindow = tick;
        m_dx = (m_isPlayerOne ? 5 : -5);
        m_punchesLanded++;

        break;
      case TAP:
        if (doDamage)
        {
          opponent.damage(2, 1, false, false);
          break;
        }
        m_punchedAlready = true;
        m_dx = (m_isPlayerOne ? 2 : -2);

        m_punchesLanded++;

        break;
      case PUSH:
        if (doDamage)
        {
          opponent.damage(0, 6, false, false);
          break;
        }
        m_punchedAlready = true;
        m_dx = (m_isPlayerOne ? 2 : -2);

        break;
      default:
        break;
    }

    m_dx = flip ? -m_dx : m_dx;

    if (doDamage)
      return true;

    if (m_super >= 250 && !hadSuper && m_aiLevel == 0)
      mmEffect(SFX_VIOLA_GET);

    if (opponent.m_aiLevel == -1)
    {
      int punchvol = (m_state == HEAVY || m_state == SUPER ? 600 : 1024);
      int sfx;

      if ((opponent.m_state == BLOCK || opponent.m_state == BLOCK_SCRIPT) && opponent.m_block > 7)
      {
        sfx = SFX_WEAK;
        punchvol = 1024;
      }
      else
      {
        sfx = SFX_LIGHT;
      }

      if (m_state == TAP || m_state == PUSH)
      {
        punchvol = 1024;
        sfx = SFX_WEAK;
      }

      int range = 100 - -100 + 1;
      int num = rand() % range + -50;

      mmEffectRate(mmEffect(sfx), punchvol + num);
      if (sfx == SFX_LIGHT)
      {
        int hurtsfx = m_state == DOWN ? opponent.sfxdown
                      : num < 0       ? opponent.sfxhurt1
                                      : opponent.sfxhurt2;
        mmEffectRate(mmEffect(hurtsfx), 1024);
      }
    }
    return true;
  }
  else
    return false;
}

bool Boxer::confirmAction(Moves state, Boxer& opponent, bool checkingSelf)
{

  if (checkingSelf && m_punchedAlready)
  {
    return false;
  }

  if (!checkingSelf && (opponent.m_damagedAlready || m_juked))
  {
    return false;
  }

  switch (state)
  {
    case LIGHT:
      return m_state == LIGHT &&
             (m_frame == m_traits->whenlight[0] || m_frame == m_traits->whenlight[1]);
      break;
    case HEAVY:
      return m_state == HEAVY && (m_frame == m_traits->whenheavy);
      break;
    case SUPER:
      return m_state == SUPER && (m_frame == m_traits->whenspecial);
      break;

      // Kairo Specific, magic numbers!
    case TAP:
      return m_state == TAP && (m_frame == 25 || m_frame == 29);
      break;
    case PUSH:
      return m_state == PUSH && (m_frame == 34);
      break;
    default:
      return false;
  }
}

void Boxer::missedPunch(Moves state, Boxer& opponent, bool flip)
{
  int push;
  int punchpitch;
  int sfx;

  m_punchWait = tick;
  m_punchedAlready = true;

  if (opponent.m_state == DODGE && opponent.m_intClock < 20)
  {
    m_punchFreq = jukedPunchFreq;
    m_intClock = 1;
    m_juked = true;
    m_blockWait = tick;
  }
  m_comboWindow = 0;
  m_punchesThrown++;

  switch (state)
  {
    case LIGHT:
      push = 3;
      sfx = SFX_LIGHTMISS;
      punchpitch = 1024;
      break;
    case HEAVY:
    case SUPER:
      push = 3;
      sfx = SFX_LIGHTMISS;
      punchpitch = 600;
      break;

      // Kairo Specific, magic numbers!

    case TAP:
    case PUSH:
      push = 1;
      sfx = SFX_LIGHTMISS;
      punchpitch = 1024;
      break;
    default:
      sfx = SFX_LIGHTMISS;
      punchpitch = 1024;
      break;
  }

  m_dx = (m_isPlayerOne ? push : -push);
  m_dx = flip ? -m_dx : m_dx;
  mmEffectRate(mmEffect(sfx), punchpitch);
}

void Boxer::superHandler(Boxer& opponent, bool roundActive)
{
  m_superActive = opponent.m_boxerLocked;

  if (m_superActive)
  {
    if (m_sequenceSuper == 0)
      Transition(1, 16, 0, false);
    m_state = SUPERSEQ;
    m_sequenceSuper++;
    for (superframe f : m_superSeq->sequence)
    {
      if (f.when == m_sequenceSuper)
      {
        m_frame = f.frame;

        if (f.sfx)
        {
          mmEffect(f.sfx);
        }

        if (f.damage > 0 && roundActive)
        {
          opponent.damage(f.damage, f.kb, true, true);
          m_dx = (m_isPlayerOne ? f.kb * 2 : -f.kb * 2);
        }

        if (f.damage == -1 || ((opponent.m_state == DOWN || !roundActive) && f.damage == -2))
        {
          m_state = IDLE;
          m_intClock = 0;
          m_indieClock = 0;
          opponent.unlock();
        }
      }
    }
  }
  else
  {
    m_sequenceSuper = -1;
  }

  if (opponent.m_aiLevel == -1)
  {
    if (m_boxerLocked)
    {
      if (mp_sequenceSuper == 0)
        Transition(1, 16, 0, false);
      mp_sequenceSuper++;
      for (superframe f : opponent.m_superSeq->sequence)
      {
        if (f.when == mp_sequenceSuper)
        {
          if (f.sfx)
          {
            mmEffect(f.sfx);
          }

          if (f.damage > 0 && roundActive)
          {
            damage(f.damage, f.kb, true, true);
          }

          if (f.damage == -1 || ((m_state == DOWN || !roundActive) && f.damage == -2))
          {
            unlock();
          }
        }
      }
    }
    else
    {
      mp_sequenceSuper = -1;
    }
  }
}

void Boxer::update(Boxer& opponent, bool roundActive, bool matchActive)
{
  // fire npc movement
  if (m_aiLevel != -1)
  {
    if (m_aiLevel > 0 && roundActive)
    {
      npcHandler(m_aiLevel, opponent);
    }

    if (m_state == DOWN && m_getUpCount > 0 && (tick % 2 == 0))
    {
      m_getUpCount -= m_downs;
      if (m_getUpCount <= 75 && m_getUpCount > 40 && m_frame > m_traits->downframes[1])
        m_frame = m_traits->getupframes[1];
      if (m_getUpCount <= 40 && m_getUpCount > 20 && m_frame > m_traits->downframes[1])
        m_frame = m_traits->getupframes[0];
    }

    movementHandler(opponent, roundActive, matchActive);
    if (roundActive)
      combatHandler(opponent);
    superHandler(opponent, roundActive);
    if (!m_superActive)
      animationHandler(matchActive, opponent);

    if (!matchActive && (m_state == BLOCK || m_state == BLOCK_SCRIPT))
    {
      m_state = IDLE;
    }
  }

  int screen_x, screen_y;
  cameraHandler(screen_x, screen_y);

  NF_MoveSprite(0, m_index, screen_x, screen_y);
  NF_SpriteFrame(0, m_index, std::clamp(m_frame, 0, m_traits->maxFrame));

  NF_SpriteFrame(0, m_index + 2, std::clamp(int(floor((40 - m_block) / 5)), 0, 8));

  if (((m_state != BLOCK && m_state != BLOCK_SCRIPT && m_block > 0)) ||
      (m_state == DOWN || opponent.m_state == DOWN) || (m_block <= 0 && tick % 2 == 0))
  {
    NF_MoveSprite(0, m_index + 2, -64, -64);
  }
}

void Boxer::killAi()
{
  m_aiLevel = 0;
}

int Boxer::getStatistics(int index)
{
  int acc = (int)((float(m_punchesLanded) / float(m_punchesLanded + m_punchesThrown)) * 100);
  int stats[3] = {m_punchesThrown, m_punchesLanded, acc};

  return stats[index];
}
