#include "Synchronizer.h"

#include "Network.h"
#include "PacketHandler.h"
#include "../Globals.h"
#include "../Player.h"
#include "../Input.h"
#include "../Ingame.h"

static u32 synchronizerLocalTurn;
static unsigned int synchronizerNextSeed;
static bool synchronizerRunning;

static int synchronizerCurrentTicks;

static bool synchronizerTurnReady();
static void synchronizerNextTurn();

static void synchronizerSendLocalInputs();

static int synchronizerGetTurnIndex(u32 turn);

void synchronizerInit(int seed, int initPlayerCount, int initPlayerLocalID) {
    synchronizerLocalTurn = 0;
    synchronizerNextSeed = seed;
    playerCount = initPlayerCount;
    playerLocalID = initPlayerLocalID;
    syncTickCount = 0;
    
    //reset player turn states (e.g. is turn data recieved), first turn needs to happen with no inputs
    for(int i=0; i<playerCount; i++) {
        resetKeys(&(players[i].inputs));
        for(int j=0; j<MAX_INPUT_BUFFER; j++) {
            resetKeys(&(players[i].nextInputs[j]));
        }
        players[i].nextTurnReady[0] = true;
        players[i].idSet = false;
        players[i].ready = false;
    }
    
    players[playerLocalID].id = localUID;
    players[playerLocalID].idSet = true;
    players[playerLocalID].ready = true;
    
    
    //switch menu
    currentMenu = MENU_LOADING;
    
    synchronizerRunning = false;
}

void synchronizerSendUID() {
    sendIDPacket(playerLocalID, localUID);
}

void synchronizerSetPlayerUID(int playerID, u32 uid) {
    players[playerID].id = uid;
    players[playerID].idSet = true;
}

void synchronizerSendIfReady() {
    //we are ready when we recieved all uids
    for(int i=0; i<playerCount; i++) {
        if(!players[i].idSet) {
            return;
        }
    }
    
    //ready -> send to server
    sendStartReadyPacket(playerLocalID);
}

void synchronizerSetPlayerReady(int playerID) {
    players[playerID].ready = true;
}

bool synchronizerAllReady() {
    for(int i=0; i<playerCount; i++) {
        if(!players[i].ready) {
            return false;
        }
    }
    return true;
}

void synchronizerStart() {
    //check if save file is present
    bool doLoad = false;
    char *loadName = NULL;
    
    //host and single player need to load the actual file
    if(playerLocalID==0) {
        FILE *file = fopen(currentFileName, "rb");
        if(file!=NULL) {
            fclose(file);
            doLoad = true;
            loadName = currentFileName;
        }
    //all others the transfered one
    } else {
        FILE *file = fopen("tmpTransfer.bin", "rb");
        if(file!=NULL) {
            fclose(file);
            doLoad = true;
            loadName = "tmpTransfer.bin";
        }
    }
    
    // reset random generators
    srand(synchronizerNextSeed);
    gaussrand(true);
    
    //start the game
    startGame(doLoad, loadName);
    
    //remove transfered save file from clients
    if(playerLocalID!=0) {
        FILE *file = fopen("tmpTransfer.bin", "rb");
        if(file!=NULL) {
            fclose(file);
            remove("tmpTransfer.bin");
        }
    }
    
    //clear menu
    currentMenu = MENU_NONE;
    
    synchronizerRunning = true;
    synchronizerCurrentTicks = SYNCHRONIZER_TICKS_PER_TURN;
}

void synchronizerTick(void (*gtick)(void)) {
    if(synchronizerRunning && synchronizerTurnReady()) {
        synchronizerNextTurn();
        
        // reset random generators
        srand(synchronizerNextSeed);
        gaussrand(true);
        
        
        syncTickCount++;
    
        //call game tick
        (*gtick)();
        
        synchronizerNextSeed = rand();
    }
}

//Test if all players (including single player) input is recieved
static bool synchronizerTurnReady() {
    if(synchronizerCurrentTicks<SYNCHRONIZER_TICKS_PER_TURN) return true;
    
    for(int i=0; i<playerCount; i++) {
        if(!players[i].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)]) {
            return false;
        }
    }
    
    return true;
}

static void synchronizerNextTurn() {
    if(synchronizerCurrentTicks<SYNCHRONIZER_TICKS_PER_TURN) {
        synchronizerCurrentTicks++;
        
        //clicked events are only fired for the first tick per turn
        for(int i=0; i<playerCount; i++) {
            resetClicked(&(players[i].inputs));
        }
    } else {        
        //move nextInput of every player to currentInput
        for(int i=0; i<playerCount; i++) {
            players[i].inputs = players[i].nextInputs[synchronizerGetTurnIndex(synchronizerLocalTurn)];
            players[i].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)] = false;
        }
        
        //Increase turn number
        synchronizerLocalTurn++;
        synchronizerCurrentTicks = 1;
        
        //send local input
        synchronizerSendLocalInputs();
    }
}

static void synchronizerSendLocalInputs() {
    //scan local inputs
    hidScanInput();
    tickKeys(&localInputs, hidKeysHeld(), hidKeysDown());
    
    //store local input in nextInput
    players[playerLocalID].nextInputs[synchronizerGetTurnIndex(synchronizerLocalTurn)] = localInputs;
    players[playerLocalID].nextTurnReady[synchronizerGetTurnIndex(synchronizerLocalTurn)] = true;
    
    //send local input
    if(playerCount>1) {
        size_t size = writeInputPacket(networkWriteBuffer, &localInputs, playerLocalID, synchronizerLocalTurn);
        networkSend(networkWriteBuffer, size);
    }
}

void synchronizerOnInputPacket(u8 playerID, u32 turnNumber, void *data, size_t dataSize) {
    if(turnNumber>=synchronizerLocalTurn && turnNumber<synchronizerLocalTurn+MAX_INPUT_BUFFER && playerID<playerCount) {
        if(readInputPacketData(data, dataSize, &(players[playerID].nextInputs[synchronizerGetTurnIndex(turnNumber)]))) {
            players[playerID].nextTurnReady[synchronizerGetTurnIndex(turnNumber)] = true;
        }
    }
}

static int synchronizerGetTurnIndex(u32 turn) {
    return turn%MAX_INPUT_BUFFER;
}

void synchronizerReset() {
    synchronizerRunning = false;
    synchronizerCurrentTicks = 0;
}

bool synchronizerIsRunning() {
    return synchronizerRunning;
}

// helpers for random numbers
//TODO: this is in a wierd place currently
#define PI 3.141592654
double gaussrand(bool reset)
{
    static double U, V;
    static int phase = 0;
    double Z;
    
    if(reset) {
        U = 0;
        V = 0;
        phase = 0;
        return 0;
    }

    if(phase == 0) {
        U = (rand() + 1.) / (RAND_MAX + 2.);
        V = rand() / (RAND_MAX + 1.);
        Z = sqrt(-2 * log(U)) * sin(2 * PI * V);
    } else {
        Z = sqrt(-2 * log(U)) * cos(2 * PI * V);
    }
        
    phase = 1 - phase;

    return Z;
}
