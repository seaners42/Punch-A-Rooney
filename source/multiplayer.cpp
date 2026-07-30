// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2025

// This example shows how to combine CMD/REPLY multiplayer packets alongisde
// regular data packets of arbitrary size.

#include "multiplayer.h"

void ResetGameState()
{
  mp_game.connected = 0;
  mp_game.gamestate = 0;
  mp_game.gametimer = 0;
  mp_game.downtimer = 0;

  mp_game.chosenMap = 0;
  mp_game.chosenRounds = 3;
  mp_game.chosenTimeSpeed = 2;

  mp_game.expectedWinner = 1;

  mp_game.player_mask = BIT(0); // The host is always enabled

  for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
  {
    mp_game.player[i].x = 86;

    // mp_gameplay Variables
    mp_game.player[i].m_hp = 100;
    mp_game.player[i].m_stamina = 100;
    mp_game.player[i].m_block = 40;
    mp_game.player[i].m_super = 0;
    mp_game.player[i].m_state = IDLE;

    mp_game.player[i].m_downs = 0;
    mp_game.player[i].m_rounddowns = 0;

    // Change x and y by
    mp_game.player[i].m_juked = false;
    mp_game.player[i].m_punchFreq = 6;
    mp_game.player[i].m_boxerLocked = false;

    mp_game.player[i].m_character = ROONEY;

    // Animation Variables
    mp_game.player[i].m_intClock = 0;
    mp_game.player[i].m_frame = 0;

    // Animation Variables
    mp_game.player[i].m_damagedAlready = false;
    mp_game.player[i].m_punchedAlready = false;
    mp_game.player[i].m_getUpCount = 0;

    mp_game.player[i].ready = false;
  }
}

void RenderGameState()
{
  for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
  {
  }
}

void SendHostStateToClients()
{
  pkt_host_to_client host_packet;

  host_packet.connected = mp_game.connected;
  host_packet.gamestate = mp_game.gamestate;
  host_packet.gametimer = mp_game.gametimer;
  host_packet.downtimer = mp_game.downtimer;

  host_packet.chosenMap = mp_game.chosenMap;
  host_packet.chosenRounds = mp_game.chosenRounds;
  host_packet.chosenTimeSpeed = mp_game.chosenTimeSpeed;

  mp_game.chosenTimeSpeed = mp_game.expectedWinner;

  host_packet.player_mask = mp_game.player_mask;

  for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
  {
    if (mp_game.player_mask & BIT(i))
    {
      host_packet.player[i].x = mp_game.player[i].x;

      // mp_gameplay Variables
      host_packet.player[i].m_hp = mp_game.player[i].m_hp;
      host_packet.player[i].m_stamina = mp_game.player[i].m_stamina;
      host_packet.player[i].m_block = mp_game.player[i].m_block;
      host_packet.player[i].m_super = mp_game.player[i].m_super;
      host_packet.player[i].m_state = mp_game.player[i].m_state;

      host_packet.player[i].m_downs = mp_game.player[i].m_downs;
      host_packet.player[i].m_rounddowns = mp_game.player[i].m_rounddowns;

      // Change x and y by
      host_packet.player[i].m_juked = mp_game.player[i].m_juked;
      host_packet.player[i].m_punchFreq = mp_game.player[i].m_punchFreq;
      host_packet.player[i].m_boxerLocked = mp_game.player[i].m_boxerLocked;

      host_packet.player[i].m_character = mp_game.player[i].m_character;

      // Animation Variables
      host_packet.player[i].m_intClock = mp_game.player[i].m_intClock;
      host_packet.player[i].m_frame = mp_game.player[i].m_frame;

      host_packet.player[i].m_damagedAlready = mp_game.player[i].m_damagedAlready;
      host_packet.player[i].m_punchedAlready = mp_game.player[i].m_punchedAlready;
      host_packet.player[i].m_getUpCount = mp_game.player[i].m_getUpCount;

      host_packet.player[i].ready = mp_game.player[i].ready;
    }
    else
    {
      // Position
      host_packet.player[i].x = 86;

      // mp_gameplay Variables
      host_packet.player[i].m_hp = 100;
      host_packet.player[i].m_stamina = 100;
      host_packet.player[i].m_block = 40;
      host_packet.player[i].m_super = 0;
      host_packet.player[i].m_state = IDLE;

      host_packet.player[i].m_downs = 0;
      host_packet.player[i].m_rounddowns = 0;

      // Change x and y by
      host_packet.player[i].m_juked = false;
      host_packet.player[i].m_punchFreq = 6;
      host_packet.player[i].m_boxerLocked = false;

      host_packet.player[i].m_character = ROONEY;

      host_packet.player[i].m_damagedAlready = false;
      host_packet.player[i].m_punchedAlready = false;

      // Animation Variables
      host_packet.player[i].m_intClock = 0;
      host_packet.player[i].m_frame = 0;

      host_packet.player[i].ready = false;
    }
  }

  Wifi_MultiplayerHostCmdTxFrame(&host_packet, sizeof(host_packet));
}

void FromHostPacketHandler(Wifi_MPPacketType type, int base, int len)
{
  if (len < sizeof(pkt_host_to_client))
  {
    // TODO: This shouldn't have happened!
    return;
  }

  if (type != WIFI_MPTYPE_CMD)
    return;

  // Save information received from the client into the global state struct
  pkt_host_to_client packet;
  Wifi_RxRawReadPacket(base, sizeof(packet), (void*)&packet);

  for (int i = 0; i < WIFI_MAX_MULTIPLAYER_CLIENTS + 1; i++)
  {
    // Position
    mp_game.player[i].x = packet.player[i].x;

    // mp_gameplay Variables
    mp_game.player[i].m_hp = packet.player[i].m_hp;
    mp_game.player[i].m_stamina = packet.player[i].m_stamina;
    mp_game.player[i].m_block = packet.player[i].m_block;
    mp_game.player[i].m_super = packet.player[i].m_super;
    mp_game.player[i].m_state = packet.player[i].m_state;

    mp_game.player[i].m_downs = packet.player[i].m_downs;
    mp_game.player[i].m_rounddowns = packet.player[i].m_rounddowns;

    // Change x and y by
    mp_game.player[i].m_juked = packet.player[i].m_juked;
    mp_game.player[i].m_boxerLocked = packet.player[i].m_boxerLocked;

    mp_game.player[i].m_character = packet.player[i].m_character;

    // Animation Variables
    mp_game.player[i].m_intClock = packet.player[i].m_intClock;
    mp_game.player[i].m_punchFreq = packet.player[i].m_punchFreq;
    mp_game.player[i].m_frame = packet.player[i].m_frame;

    mp_game.player[i].m_damagedAlready = packet.player[i].m_damagedAlready;
    mp_game.player[i].m_punchedAlready = packet.player[i].m_punchedAlready;
    mp_game.player[i].m_getUpCount = packet.player[i].m_getUpCount;

    mp_game.player[i].ready = packet.player[i].ready;
  }

  mp_game.player_mask = packet.player_mask;
  mp_game.gametimer = packet.gametimer;
  mp_game.gamestate = packet.gamestate;

  mp_game.downtimer = packet.downtimer;

  mp_game.expectedWinner = packet.expectedWinner;

  mp_game.chosenMap = packet.chosenMap;
  mp_game.chosenRounds = packet.chosenRounds;
  mp_game.chosenTimeSpeed = packet.chosenTimeSpeed;

  mp_game.connected = packet.connected;
  lastCom = 0;
}

// Client to host packets
// ======================

void FromClientPacketHandler(Wifi_MPPacketType type, int aid, int base, int len)
{
  if (len < sizeof(pkt_client_to_host))
  {
    // TODO: This shouldn't have happened!
    return;
  }

  if (type != WIFI_MPTYPE_REPLY)
    return;

  // Save information received from the client into the global state struct
  pkt_client_to_host packet;
  Wifi_RxRawReadPacket(base, sizeof(packet), (void*)&packet);

  // Position
  mp_game.player[aid].x = packet.x;

  // mp_gameplay Variables
  mp_game.player[aid].m_hp = packet.m_hp;
  mp_game.player[aid].m_stamina = packet.m_stamina;
  mp_game.player[aid].m_block = packet.m_block;
  mp_game.player[aid].m_super = packet.m_super;
  mp_game.player[aid].m_state = packet.m_state;

  mp_game.player[aid].m_downs = packet.m_downs;
  mp_game.player[aid].m_rounddowns = packet.m_rounddowns;

  // Change x and y by
  mp_game.player[aid].m_juked = packet.m_juked;
  mp_game.player[aid].m_boxerLocked = packet.m_boxerLocked;

  mp_game.player[aid].m_character = packet.m_character;

  // Animation Variables
  mp_game.player[aid].m_intClock = packet.m_intClock;
  mp_game.player[aid].m_frame = packet.m_frame;
  mp_game.player[aid].m_punchFreq = packet.m_punchFreq;

  mp_game.player[aid].m_damagedAlready = packet.m_damagedAlready;
  mp_game.player[aid].m_punchedAlready = packet.m_punchedAlready;
  mp_game.player[aid].m_getUpCount = packet.m_getUpCount;

  mp_game.player[aid].ready = packet.ready;

  mp_game.player_mask |= BIT(aid);
  lastCom = 0;
}

void start_host()
{
  lastCom = 0;
  mp_game.gamestate = 0;

  Wifi_MultiplayerHostMode(15, sizeof(pkt_host_to_client), sizeof(pkt_client_to_host));

  ResetGameState();

  Wifi_MultiplayerFromClientSetPacketHandler(FromClientPacketHandler);

  while (!Wifi_LibraryModeReady())
    swiWaitForVBlank();

  // You can call the next functions before loading a beacon to set up the
  // beacon. They can also be called afterwards.
  Wifi_SetChannel(10);
  Wifi_MultiplayerAllowNewClients(true);

  Wifi_BeaconStart("NintendoDS", 0xCAFEF00D);

  swiWaitForVBlank();
  swiWaitForVBlank();

  mpStatus = MP_HOST_SEARCHING;
  isHost = true;
  roomWasSelected = false;
}

void start_client()
{
  lastCom = 0;
  mp_game.gamestate = 0;

  Wifi_MultiplayerClientMode(sizeof(pkt_client_to_host));

  while (!Wifi_LibraryModeReady())
    swiWaitForVBlank();

  Wifi_ScanMode();

  mpStatus = MP_CLIENT_SEARCHING;
  isHost = false;
  roomWasSelected = false;
}

void selecting_a_room()
{
  swiWaitForVBlank();

  // Get find out how many APs there are in the area
  int count = Wifi_GetNumAP();

  if (count == 0)
    return;

  int whereToGo = 0;

  Wifi_AccessPoint ap;
  Wifi_GetAPData(whereToGo, &ap);

  // In multiplayer client mode DSWiFi ignores all access points that
  // don't offer any Nintendo information. Also, DSWiFi host access
  // points don't use any encryption.

  AccessPoint = ap;

  Wifi_MultiplayerFromHostSetPacketHandler(FromHostPacketHandler);
  Wifi_ConnectOpenAP(&AccessPoint);

  int oldstatus = -1;
  while (1)
  {
    swiWaitForVBlank();
    setBrightness(3, 0);

    scanKeys();
    if (keysDown())
      return;

    int status = Wifi_AssocStatus();

    if (status != oldstatus)
    {
      printf("%s\n", ASSOCSTATUS_STRINGS[status]);
      oldstatus = status;
    }

    if (status == ASSOCSTATUS_CANNOTCONNECT)
    {
      mpStatus = MP_CONNECTION_LOST;
      disable_multiplayer();
      return;
    }

    if (status == ASSOCSTATUS_ASSOCIATED)
    {
      ResetGameState();
      roomWasSelected = true;
      break;
    }
  }
  return;
}
void disable_multiplayer()
{
  Wifi_DisconnectAP();
  Wifi_IdleMode();
  mp_game.connected = 0;
  mpStatus = MP_CONNECTION_LOST;
}

void tick_multiplayer()
{
  if (mp_game.connected == 1)
  {
    if (isHost)
    {
      SendHostStateToClients();
    }
    lastCom++;
    if (lastCom > 300)
    {
      mp_game.connected = 0;
      mpStatus = MP_CONNECTION_LOST;
      disable_multiplayer();
    }
  }

  if (!isHost && !roomWasSelected)
  {
    selecting_a_room();
  }

  if (isHost)
  {
    int num_clients = Wifi_MultiplayerGetNumClients();

    Wifi_ConnectedClient client[2];
    num_clients = Wifi_MultiplayerGetClients(2, &(client[0]));

    if (num_clients > 0)
    {
      mp_game.connected = 1;
      mpStatus = MP_HOST_READY;
    }

    if (num_clients < 1 && mp_game.connected == 1)
    {
      mp_game.connected = 0;
      mpStatus = MP_CONNECTION_LOST;
      disable_multiplayer();
    }
  }
}

int get_multiplayer_status()
{
  return mpStatus;
}