#pragma once

#include <dswifi9.h>
#include <nds.h>
#include <nf_lib.h>

#include "enums.h"

static Wifi_AccessPoint AccessPoint;

// Host to client packets
// ======================
#define MAX_CLIENTS 2

/*
typedef struct {

    // Host Started

    // Position
    int m_x;
    int m_y;
    int m_rot;

    // Gameplay Variables
    int m_hp;
    int m_stamina;
    int m_block;
    int m_super;
    Moves m_state;

    int m_downs;

    // Change x and y by
    bool m_juked;
    bool m_boxerLocked;

    Character m_character;

    // Animation Variables
    int m_intClock;
    int m_frame;

    // Get Up Count handles if you get up after a knockdown
    int m_getUpCount;

    bool ready;


} Client;

typedef struct {
    // Host Started

    // Position
    int m_x;
    int m_y;
    int m_rot;

    // Gameplay Variables
    int m_hp;
    int m_stamina;
    int m_block;
    int m_super;
    Moves m_state;

    int m_downs;

    // Change x and y by
    bool m_juked;
    bool m_boxerLocked;

    Character m_character;

    // Animation Variables
    int m_intClock;
    int m_frame;

    // Get Up Count handles if you get up after a knockdown
    int m_getUpCount;

    bool ready;
} pkt_client_to_host;
*/

typedef struct
{
  // Position
  int x;

  // Gameplay Variables
  int m_hp;
  int m_stamina;
  int m_block;
  int m_super;
  Moves m_state;

  int m_downs;
  int m_rounddowns;

  // Change x and y by
  bool m_juked;
  bool m_boxerLocked;

  Character m_character;

  // Animation Variables
  int m_intClock;
  int m_frame;
  int m_punchFreq;

  // combat vars
  bool m_damagedAlready;
  bool m_punchedAlready;
  int m_getUpCount;

  bool ready;
} Client;

typedef struct
{
  // Position
  int x;

  // Gameplay Variables
  int m_hp;
  int m_stamina;
  int m_block;
  int m_super;
  Moves m_state;

  int m_downs;
  int m_rounddowns;

  // Change x and y by
  bool m_juked;
  bool m_boxerLocked;

  Character m_character;

  // Animation Variables
  int m_intClock;
  int m_punchFreq;
  int m_frame;

  // combat vars
  bool m_damagedAlready;
  bool m_punchedAlready;
  int m_getUpCount;

  bool ready;
} pkt_client_to_host;

typedef struct
{
  // This is set to 1 when the game has started, it's kept as 0 while the host
  // waits for new clients to be connected.
  u8 connected;

  int gamestate;
  int gametimer;
  int downtimer;

  int chosenMap;
  int chosenRounds;
  int chosenTimeSpeed;

  int expectedWinner;

  // Bit 0 is set to 1 when the host is connected (always). Bits 1 to 7 are
  // set to 1 if a client with that AID is connected. The maximum number of
  // clients allowed is MAX_CLIENTS (7).
  u8 player_mask;

  // Information of all Clients plus host
  Client player[WIFI_MAX_MULTIPLAYER_CLIENTS + 1];
} game_info;

typedef struct
{

  u8 connected;
  int gamestate;
  int gametimer;
  int downtimer;

  int chosenMap;
  int chosenRounds;
  int chosenTimeSpeed;

  int expectedWinner;

  u8 player_mask;
  Client player[WIFI_MAX_MULTIPLAYER_CLIENTS + 1];

} pkt_host_to_client;

enum MultiplayerStatus
{
  MP_HOST_SEARCHING = 0,
  MP_CLIENT_SEARCHING = 1,
  MP_HOST_READY = 2,
  MP_CLIENT_READY = 3,
  MP_CONNECTION_LOST = 4
};

inline game_info mp_game;

static int mpStatus = MP_CLIENT_SEARCHING;
static int lastCom = 0;
inline bool isHost = false;
static bool roomWasSelected = false;

void FromHostPacketHandler(Wifi_MPPacketType type, int base, int len);
void FromClientPacketHandler(Wifi_MPPacketType type, int aid, int base, int len);
void SendHostStateToClients();

void createRoom();

void host_mode();
bool access_point_selection_menu();
void client_mode();

void selecting_a_room();
void start_host();
void start_client();
void tick_multiplayer();

int get_multiplayer_status();

void disable_multiplayer();