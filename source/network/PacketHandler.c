#include "PacketHandler.h"

#include <stdlib.h>
#include "Synchronizer.h"

static FILE *recvFile;
static size_t recvFileSize;




static void * writeBool(void *buffer, size_t *size, bool value) {
    *((bool*) buffer) = value;
    *(size) += sizeof(bool);
    return buffer + sizeof(bool);
}

static void * writeU8(void *buffer, size_t *size, u8 value) {
    *((u8*) buffer) = value;
    *(size) += sizeof(u8);
    return buffer + sizeof(u8);
}

static void * writeU16(void *buffer, size_t *size, u16 value) {
    *((u16*) buffer) = value;
    *(size) += sizeof(u16);
    return buffer + sizeof(u16);
}

static void * writeU32(void *buffer, size_t *size, u32 value) {
    *((u32*) buffer) = value;
    *(size) += sizeof(u32);
    return buffer + sizeof(u32);
}

static void * writeSizeT(void *buffer, size_t *size, size_t value) {
    *((size_t*) buffer) = value;
    *(size) += sizeof(size_t);
    return buffer + sizeof(size_t);
}




static void * readBool(void *buffer, size_t *size, bool *value) {
    *value = *((bool*) buffer);
    *(size) -= sizeof(bool);
    return buffer + sizeof(bool);
}

static void * readU8(void *buffer, size_t *size, u8 *value) {
    *value = *((u8*) buffer);
    *(size) -= sizeof(u8);
    return buffer + sizeof(u8);
}

static void * readU16(void *buffer, size_t *size, u16 *value) {
    *value = *((u16*) buffer);
    *(size) -= sizeof(u16);
    return buffer + sizeof(u16);
}

static void * readU32(void *buffer, size_t *size, u32 *value) {
    *value = *((u32*) buffer);
    *(size) -= sizeof(u32);
    return buffer + sizeof(u32);
}

static void * readSizeT(void *buffer, size_t *size, size_t *value) {
    *value = *((size_t*) buffer);
    *(size) -= sizeof(size_t);
    return buffer + sizeof(size_t);
}




void processPacket(void *packet, size_t size) {
    //Differenciate the packets and process them
    switch(packetGetID(packet)) {
    case PACKET_START: {
        void *buffer = packetGetDataStart(packet);
        size = packetGetDataSize(size);
        
        //find player index based on network node id
        //and set player uuid in synchronizer
        u32 seed;
        u32 playerCount = 1;
        int playerIndex = 0;
        
        buffer = readU32(buffer, &size, &seed);
        buffer = readU32(buffer, &size, &playerCount);
        for(int i=0; i<playerCount; i++) {
            u16 nodeID;
            
            buffer = readU16(buffer, &size, &nodeID);
            
            if(nodeID==networkGetLocalNodeID()) {
                playerIndex = i;
            }
        }
        
        //cleanup transfer tmp file
        FILE *file = fopen("tmpTransfer.bin", "wb");
        if(file!=NULL) {
            fclose(file);
            remove("tmpTransfer.bin");
        }
        
        //init synchronizer
        synchronizerInit(seed, playerCount, playerIndex);
        
        break;
    }
    case PACKET_START_FILEHEADER: {
        void *data = packetGetDataStart(packet);
        
        u8 type;
        u8 id;
        size_t fsize;
        
        data = readU8(data, &size, &type);
        data = readU8(data, &size, &id);
        data = readSizeT(data, &size, &fsize);
    
        recvFile = fopen("tmpTransfer.bin", "wb");
        recvFileSize = fsize;
    
        break;
    }
    case PACKET_START_FILEDATA: {
        void *data = packetGetDataStart(packet);
        size_t dsize = packetGetDataSize(size);
        
        size_t toread = dsize;
        
        fwrite(data, 1, toread, recvFile);
        
        recvFileSize -= toread;
        if(recvFileSize<=0) {
            fclose(recvFile);
            recvFile = NULL;
        }
        
        break;
    }
    case PACKET_START_REQUEST_IDS: {
        synchronizerSendUID();
        break;
    }
    case PACKET_START_ID: {
        u8 playerID = packetGetSender(packet);
        u32 uid;
        
        void *data = packetGetDataStart(packet);
        size_t dsize = packetGetDataSize(size);
        
        data = readU32(data, &dsize, &uid);
        
        synchronizerSetPlayerUID(playerID, uid);
        synchronizerSendIfReady();
        break;
    }
    case PACKET_START_READY: {
        synchronizerSetPlayerReady(packetGetSender(packet));
        if(playerLocalID==0) {
            if(synchronizerAllReady()) {
                sendStartSyncPacket();
                synchronizerStart(); //server needs to call this here, all others do when they recieve the packet
            }
        }
        break;
    }
    
    case PACKET_TURN_START: {
        synchronizerStart();
        break;
    }
    case PACKET_TURN_INPUT: {
        synchronizerOnInputPacket(packetGetSender(packet), packetGetTurn(packet), packetGetDataStart(packet), packetGetDataSize(size));
        break;
    }
    }
}

u8 packetGetID(void *packet) {
    return *((u8*) packet);
}

u8 packetGetSender(void *packet) {
    return *((u8*) packet+sizeof(u8));
}

u32 packetGetTurn(void *packet) {
    return *((u32*) (packet+sizeof(u8)+sizeof(u8)));
}

void * packetGetDataStart(void *packet) {
    return packet+sizeof(u8)+sizeof(u8)+sizeof(u32);
}

size_t packetGetDataSize(size_t size) {
    return size-sizeof(u8)-sizeof(u8)-sizeof(u32);
}




size_t writeStartPacket(void *buffer, u32 seed) {
    size_t size = 0;
    buffer = writeU8(buffer, &size, PACKET_START);
    buffer = writeU8(buffer, &size, 0);
    buffer = writeU32(buffer, &size, 0);
    
    buffer = writeU32(buffer, &size, seed);
    buffer = writeU32(buffer, &size, (u32)networkGetNodeCount());
    for(int i=1; i<=UDS_MAXNODES; i++) {
        if(networkIsNodeConnected(i)) {
            buffer = writeU16(buffer, &size, (u16)i);
        }
    }
    
    return size;
}

size_t writeStartRequestPacket(void *buffer) {
    size_t size = 0;
    buffer = writeU8(buffer, &size, PACKET_START_REQUEST_IDS);
    buffer = writeU8(buffer, &size, 0);
    buffer = writeU32(buffer, &size, 0);
    
    return size;
}

size_t writeInputPacket(void *buffer, Inputs *inputs, u8 playerID, u32 turnNumber) {
    size_t size = 0;
    buffer = writeU8(buffer, &size, PACKET_TURN_INPUT);
    buffer = writeU8(buffer, &size, playerID);
    buffer = writeU32(buffer, &size, turnNumber);
    
    buffer = writeU16(buffer, &size, inputs->k_touch.px);
    buffer = writeU16(buffer, &size, inputs->k_touch.py);
    
    buffer = writeBool(buffer, &size, inputs->k_up.down); buffer = writeBool(buffer, &size, inputs->k_up.clicked);
    buffer = writeBool(buffer, &size, inputs->k_down.down); buffer = writeBool(buffer, &size, inputs->k_down.clicked);
    buffer = writeBool(buffer, &size, inputs->k_left.down); buffer = writeBool(buffer, &size, inputs->k_left.clicked);
    buffer = writeBool(buffer, &size, inputs->k_right.down); buffer = writeBool(buffer, &size, inputs->k_right.clicked);
    buffer = writeBool(buffer, &size, inputs->k_attack.down); buffer = writeBool(buffer, &size, inputs->k_attack.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menu.down); buffer = writeBool(buffer, &size, inputs->k_menu.clicked);
    buffer = writeBool(buffer, &size, inputs->k_pause.down); buffer = writeBool(buffer, &size, inputs->k_pause.clicked);
    buffer = writeBool(buffer, &size, inputs->k_accept.down); buffer = writeBool(buffer, &size, inputs->k_accept.clicked);
    buffer = writeBool(buffer, &size, inputs->k_decline.down); buffer = writeBool(buffer, &size, inputs->k_decline.clicked);
    buffer = writeBool(buffer, &size, inputs->k_delete.down); buffer = writeBool(buffer, &size, inputs->k_delete.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menuNext.down); buffer = writeBool(buffer, &size, inputs->k_menuNext.clicked);
    buffer = writeBool(buffer, &size, inputs->k_menuPrev.down); buffer = writeBool(buffer, &size, inputs->k_menuPrev.clicked);
    
    return size;
}

bool readInputPacketData(void *buffer, size_t size, Inputs *inputs) {
    buffer = readU16(buffer, &size, &(inputs->k_touch.px));
    if(size<=0) return false;
    buffer = readU16(buffer, &size, &(inputs->k_touch.py));
    if(size<=0) return false;
    
    buffer = readBool(buffer, &size, &(inputs->k_up.down)); buffer = readBool(buffer, &size, &(inputs->k_up.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_down.down)); buffer = readBool(buffer, &size, &(inputs->k_down.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_left.down)); buffer = readBool(buffer, &size, &(inputs->k_left.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_right.down)); buffer = readBool(buffer, &size, &(inputs->k_right.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_attack.down)); buffer = readBool(buffer, &size, &(inputs->k_attack.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_menu.down)); buffer = readBool(buffer, &size, &(inputs->k_menu.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_pause.down)); buffer = readBool(buffer, &size, &(inputs->k_pause.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_accept.down)); buffer = readBool(buffer, &size, &(inputs->k_accept.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_decline.down)); buffer = readBool(buffer, &size, &(inputs->k_decline.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_delete.down)); buffer = readBool(buffer, &size, &(inputs->k_delete.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_menuNext.down)); buffer = readBool(buffer, &size, &(inputs->k_menuNext.clicked));
    if(size<=0) return false;
    buffer = readBool(buffer, &size, &(inputs->k_menuPrev.down)); buffer = readBool(buffer, &size, &(inputs->k_menuPrev.clicked));

    return size==0;
}

void sendFile(FILE *file, u8 fileType, u8 id) {
    fseek(file, 0, SEEK_END); // seek to end of file
    size_t fsize = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    
    //send file header
    void *buffer = networkWriteBuffer;
    size_t size = 0;
    buffer = writeU8(buffer, &size, PACKET_START_FILEHEADER);
    buffer = writeU8(buffer, &size, 0);
    buffer = writeU32(buffer, &size, 0);
    buffer = writeU8(buffer, &size, fileType);
    buffer = writeU8(buffer, &size, id);
    buffer = writeSizeT(buffer, &size, fsize);
    networkSend(networkWriteBuffer, size);
    
    //send file data
    while(fsize>0) {
        buffer = networkWriteBuffer;
        size = 0;
        buffer = writeU8(buffer, &size, PACKET_START_FILEDATA);
        buffer = writeU8(buffer, &size, 0);
        buffer = writeU32(buffer, &size, 0);
 
        //read file data
        size_t towrite = NETWORK_MAXDATASIZE - size;
        if(towrite>fsize) towrite = fsize;
        
        fread(buffer, 1, towrite, file);
        
        size += towrite;
        fsize -= towrite;
        
        //send file data
        networkSend(networkWriteBuffer, size);
    }
}

void sendIDPacket(u8 playerID, u32 uid) {
    void *buffer = networkWriteBuffer;
    size_t size = 0;
    
    buffer = writeU8(buffer, &size, PACKET_START_ID);
    buffer = writeU8(buffer, &size, playerID);
    buffer = writeU32(buffer, &size, 0);
    
    buffer = writeU32(buffer, &size, uid);
    
    networkSend(networkWriteBuffer, size);
}

void sendStartReadyPacket(u8 playerID) {
    void *buffer = networkWriteBuffer;
    size_t size = 0;
    
    buffer = writeU8(buffer, &size, PACKET_START_READY);
    buffer = writeU8(buffer, &size, playerID);
    buffer = writeU32(buffer, &size, 0);
    
    networkSend(networkWriteBuffer, size);
}

void sendStartSyncPacket() {
    void *buffer = networkWriteBuffer;
    size_t size = 0;
    
    buffer = writeU8(buffer, &size, PACKET_TURN_START);
    buffer = writeU8(buffer, &size, 0);
    buffer = writeU32(buffer, &size, 0);
    
    networkSend(networkWriteBuffer, size);
}
