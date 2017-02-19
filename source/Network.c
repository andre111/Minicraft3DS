#include "Network.h"

#include "PacketHandler.h"
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

bool udsRunning;

size_t scannedNetworksCount;
udsNetworkScanInfo *scannedNetworks;

bool isConnected;
bool isServer;

size_t networkBufferSize;
u32 *networkBuffer;

udsNetworkStruct networkStruct;
udsBindContext networkBindCtx;

void networkInit() {
	Result ret = udsInit(0x3000, NULL);
	if(R_FAILED(ret)) {
		udsRunning = false;
	} else {
		udsRunning = true;
		
		scannedNetworksCount = 0;
		scannedNetworks = NULL;
		isConnected = false;
		isServer = false;

		networkBufferSize = 0x4000;		
		networkBuffer = malloc(networkBufferSize);
	}
}

void networkExit() {
	//TODO: Additionally to shutting down the service, clear any left over memory!
	if(udsRunning) {
		//cleanup any dynamically reserved memory
		if(scannedNetworks!=NULL) free(scannedNetworks);
		scannedNetworks = NULL;
		
		free(networkBuffer);
		networkDisconnect();
		
		udsExit();
	}
}



bool networkAvailable() {
	return udsRunning;
}



bool networkHost() {
	if(udsRunning && !isConnected) {
		udsGenerateDefaultNetworkStruct(&networkStruct, NETWORK_WLANCOMMID, 0, NETWORK_MAXPLAYERS);
		
		Result ret = udsCreateNetwork(&networkStruct, NETWORK_PASSPHRASE, strlen(NETWORK_PASSPHRASE)+1, &networkBindCtx, NETWORK_CHANNEL, NETWORK_RECVBUFSIZE);
		if(R_FAILED(ret)) {
			return false;
		} else {
			isConnected = true;
			isServer = true;
			return true;
		}
	}
	return false;
}

void networkScan() {
	if(udsRunning) {
		//reset values from last scan
		if(scannedNetworks!=NULL) free(scannedNetworks);
		scannedNetworks = NULL;
		scannedNetworksCount = 0;
		
		//scan
		memset(networkBuffer, 0, networkBufferSize);
		Result ret = udsScanBeacons(networkBuffer, networkBufferSize, &scannedNetworks, &scannedNetworksCount, NETWORK_WLANCOMMID, 0, NULL, isConnected);
		if(R_FAILED(ret)) {
			//TODO: what do?
			scannedNetworksCount = 0;
		}
	}
}

int networkGetScanCount() {
	if(udsRunning) {
		return scannedNetworksCount;
	} else {
		return 0;
	}
}

bool networkGetScanName(char *name, int pos) {
	if(udsRunning) {
		if(pos<0 || pos>=scannedNetworksCount) return false;
		
		Result ret = udsGetNodeInfoUsername(&scannedNetworks[pos].nodes[0], name);
		if(R_FAILED(ret)) {
			//TODO: what do?
			return false;
		}
		return true;
	}
	return false;
}

bool networkConnect(int pos) {
	if(udsRunning && !isConnected) {
		if(pos<0 || pos>=scannedNetworksCount) return false;
		
		Result ret = udsConnectNetwork(&scannedNetworks[pos].network, NETWORK_PASSPHRASE, strlen(NETWORK_PASSPHRASE)+1, &networkBindCtx, UDS_BROADCAST_NETWORKNODEID, UDSCONTYPE_Client, NETWORK_CHANNEL, NETWORK_RECVBUFSIZE);
		if(R_FAILED(ret)) {
			return false;
		} else {
			isConnected = true;
			isServer = false;
			return true;
		}
	}
	return false;
}

void networkDisconnect() {
	//TODO: For clients this just means disconnect, for the server it means destroy the network
	if(udsRunning && isConnected) {
		//TODO
		if(isServer) {
			udsDisconnectNetwork();
		} else {
			//TODO: Clients need to cleanup too, how can I tell they got disconnected
			udsDestroyNetwork();
		}
		udsUnbind(&networkBindCtx);
		
		isConnected = false;
	}
}



bool networkConnected() {
	return isConnected;
} 



void networkSend(networkPacket *packet, size_t size) {
	if(udsRunning && isConnected) {
		//Server broadcasts to all clients but clients only send info to server
		u16 dest;
		if(isServer) {
			dest = UDS_BROADCAST_NETWORKNODEID;
		} else {
			dest = UDS_HOST_NETWORKNODEID;
		}
		
		Result ret = udsSendTo(dest, NETWORK_CHANNEL, UDS_SENDFLAG_Default, packet, size);
		if(UDS_CHECK_SENDTO_FATALERROR(ret)) {
			//TODO: what do?
		}
	}
}

void networkRecieve() {
	//TODO: Should recieve actually handle all the packets or just return them?
	if(udsRunning && isConnected) {
		size_t actualSize = 0;
		u16 sourceNetworkNodeID;
		
		memset(networkBuffer, 0, networkBufferSize);
		
		Result ret = udsPullPacket(&networkBindCtx, networkBuffer, networkBufferSize, &actualSize, &sourceNetworkNodeID);
		if(R_FAILED(ret)) {
			//TODO: what do?
		}
		
		//actualSize will be 0 if no packet is available
		if(actualSize) {
			networkPacket *packet = (networkPacket*) networkBuffer;
			
			processPacket(packet, packet->analyze.type);
		}
	}
} 
