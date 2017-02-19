#pragma once

#include <3ds.h>

#define NETWORK_WLANCOMMID 0x11441850
#define NETWORK_PASSPHRASE "minicraft3ds localplay passphrase"
#define NETWORK_CHANNEL 1

#define NETWORK_RECVBUFSIZE UDS_DEFAULT_RECVBUFSIZE

#define NETWORK_MAXPLAYERS UDS_MAXNODES

//packet type ids
#define PACKET_REQUEST_MAPDATA 1
#define PACKET_MAPDATA 2

//TODO: Every other packet struct should start with a u8 type to match this
typedef struct {
	u8 type;
} packetAnalyze;

typedef struct {
    u8 type;
    
    u8 level;
} packetRequestMapData;

typedef struct {
    u8 type;
    
    u8 level;
    u8 offset; //-> data is at offset*128 to offset*128+127
    
    u8 map[128];
    u8 data[128];
} packetMapData;

typedef struct {
	union {
		packetAnalyze analyze;
		packetRequestMapData requestMapData;
        packetMapData mapData;
	};
} networkPacket;

void networkInit();
void networkExit();

bool networkAvailable();

bool networkHost();
void networkScan();
int networkGetScanCount();
bool networkGetScanName(char *name, int pos); //TODO: Name should be long enough to handle all allowed names (char[256])
bool networkConnect(int pos);
void networkDisconnect();

bool networkConnected();

void networkSend(networkPacket *packet, size_t size); //TODO: Should this be a pointer? Calling function needs to cleanup itself
void networkRecieve(); //TODO: Should recieve actually handle all the packets or just return them?
