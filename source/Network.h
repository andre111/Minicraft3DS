#pragma once

#include <3ds.h>

#define NETWORK_WLANCOMMID 0x11441850
#define NETWORK_PASSPHRASE "minicraft3dsLP"
#define NETWORK_CHANNEL 1

#define NETWORK_RECVBUFSIZE UDS_DEFAULT_RECVBUFSIZE

#define NETWORK_MAXDATASIZE 1024
#define NETWORK_SENDBUFFERSIZE ((NETWORK_MAXDATASIZE+256)*10)

#define NETWORK_STACKSIZE (8*1024)

#define NETWORK_MAXPLAYERS 8

void *networkWriteBuffer;

void networkInit();
void networkExit();

bool networkAvailable();

bool networkHost();
void networkHostStopConnections();
void networkScan();
int networkGetScanCount();
bool networkGetScanName(char *name, int pos);
bool networkConnect(int pos);
void networkDisconnect();

void networkStart();

void networkUpdateStatus();
bool networkConnected();

int networkGetNodeCount();
u16 networkGetLocalNodeID();
bool networkIsNodeConnected(u16 id);
bool networkGetNodeName(u16 id, char *name);

u16 networkGetExpectedSeqFrom(u16 id);
bool networkSeqIsLowerThan(u16 firstID, u16 secondID);

void networkSend(void *packet, size_t size);
void networkSendWaitFlush();
