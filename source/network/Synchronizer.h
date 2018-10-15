#pragma once

#include <3ds.h>

//2-3 seem be optimal (at least for 2 players)
#define SYNCHRONIZER_TICKS_PER_TURN 2

void synchronizerInit(int seed, int initPlayerCount, int initPlayerLocalID);

void synchronizerSendUID();
void synchronizerSetPlayerUID(int playerID, u32 uid);
void synchronizerSendIfReady();
void synchronizerSetPlayerReady(int playerID);
bool synchronizerAllReady();
bool synchronizerIsRunning();

void synchronizerStart();

void synchronizerTick(void (*gtick)(void));

void synchronizerReset();

void synchronizerOnInputPacket(u8 playerID, u32 turnNumber, void *data, size_t dataSize);

// values used ingame
u32 syncTickCount;

// helpers for random numbers
double gaussrand(bool reset);
