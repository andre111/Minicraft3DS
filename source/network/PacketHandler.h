#pragma once

#include <stdlib.h>
#include "Network.h"
#include "../Player.h"

#define PACKET_START 0
#define PACKET_START_FILEHEADER 1
#define PACKET_START_FILEDATA 2
#define PACKET_START_REQUEST_IDS 3
#define PACKET_START_ID 4
#define PACKET_START_READY 5

#define PACKET_TURN_START 10
#define PACKET_TURN_INPUT 11

void processPacket(void *packet, size_t size);

u8 packetGetID(void *packet);
u8 packetGetSender(void *packet);
u32 packetGetTurn(void *packet);
void * packetGetDataStart(void *packet);
size_t packetGetDataSize(size_t size);

size_t writeStartPacket(void *buffer, u32 seed);
size_t writeStartRequestPacket(void *buffer);

size_t writeInputPacket(void *buffer, Inputs *inputs, u8 playerID, u32 turnNumber);
bool readInputPacketData(void *buffer, size_t size, Inputs *inputs);

void sendFile(FILE *file, u8 fileType, u8 id);
void sendIDPacket(u8 playerID, u32 uid);
void sendStartReadyPacket(u8 playerID);
void sendStartSyncPacket();
