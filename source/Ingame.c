#include "Ingame.h"

#include "Globals.h"
#include "Player.h"
#include "Entity.h"
#include "IngameMenu.h"
#include "Render.h"
#include "MapGen.h"
#include "SaveLoad.h"
#include "network/Network.h"
#include "network/Synchronizer.h"

#define STALL_TIME 120

int stallCounter;
bool stallAreYouSure;

//generates stairs up and creates compass data
void generatePass2() {
    int level, x, y;
    
    for (level = 0; level < 5; ++level) {
        for (x = 0; x < 128; ++x) {
            for (y = 0; y < 128; ++y) {

                //generate stairs up matching stairs down
                switch (worldData.map[level][x + y * 128]) {
                case TILE_STAIRS_DOWN:
                    if(level < 4) {
                        worldData.map[level + 1][x + y * 128] = TILE_STAIRS_UP;
                        if (level == 0) {
                            worldData.map[level + 1][(x + 1) + y * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][x + (y + 1) * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][(x - 1) + y * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][x + (y - 1) * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][(x + 1) + (y + 1) * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][(x - 1) + (y - 1) * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][(x - 1) + (y + 1) * 128] = TILE_HARDROCK;
                            worldData.map[level + 1][(x + 1) + (y - 1) * 128] = TILE_HARDROCK;
                        } else {
                            worldData.map[level + 1][(x + 1) + y * 128] = TILE_DIRT;
                            worldData.map[level + 1][x + (y + 1) * 128] = TILE_DIRT;
                            worldData.map[level + 1][(x - 1) + y * 128] = TILE_DIRT;
                            worldData.map[level + 1][x + (y - 1) * 128] = TILE_DIRT;
                            worldData.map[level + 1][(x + 1) + (y + 1) * 128] = TILE_DIRT;
                            worldData.map[level + 1][(x - 1) + (y - 1) * 128] = TILE_DIRT;
                            worldData.map[level + 1][(x - 1) + (y + 1) * 128] = TILE_DIRT;
                            worldData.map[level + 1][(x + 1) + (y - 1) * 128] = TILE_DIRT;
                        }
                    }
                }
                    
                //calculate compass data
                //choose one stair down and store for magic compass
                switch (worldData.map[level][x + y * 128]) {
                case TILE_STAIRS_DOWN:
                case TILE_DUNGEON_ENTRANCE:
                    worldData.compassData[level][2] = worldData.compassData[level][2] + 1;
                    if((worldData.compassData[level][2]==1) || (rand()%(worldData.compassData[level][2])==0)) {
                        worldData.compassData[level][0] = x;
                        worldData.compassData[level][1] = y;
                    }
                }
            }
        }
    }
}

void initNewMap() {
    createAndValidateSkyMap(128, 128, 0, worldData.map[0], worldData.data[0]);
    createAndValidateTopMap(128, 128, 1, worldData.map[1], worldData.data[1]);
    createAndValidateUndergroundMap(128, 128, 1, 2, worldData.map[2], worldData.data[2]);
    createAndValidateUndergroundMap(128, 128, 2, 3, worldData.map[3], worldData.data[3]);
    createAndValidateUndergroundMap(128, 128, 3, 4, worldData.map[4], worldData.data[4]);
    generatePass2();
}

void initMiniMap(PlayerData *pd) {
    int i;
    for (i = 0; i < 5; ++i) {
        initMinimapLevel(pd, i);
    }
}

void startGame(bool load, char *filename) {
    // Reset entity manager.
    memset(&eManager, 0, sizeof(eManager));
    
    // Reset players
    for(int i=0; i<playerCount; i++) {
        initPlayer(players+i);
    }
    
    if (!load) {
        initNewMap();
        airWizardHealthDisplay = 2000;
        int i;
        for (i = 0; i < 5; ++i) {
            trySpawn(500, i);
        }
        addEntityToList(newEntityAirWizard(630, 820, 0), &eManager);
        worldData.daytime = 6000;
        worldData.day = 0;
        worldData.season = 0;
        worldData.rain = false;
    } else {
        if(!loadWorld(filename, &eManager, &worldData, players, playerCount)) {
            //TODO: What do?
            networkDisconnect();
                    
            sf2d_set_clear_color(0xFF);
            currentSelection = 0;
            currentMenu = MENU_TITLE;
        }
    }
    
    // Spawn players
    for(int i=0; i<playerCount; i++) {
        if(!players[i].isSpawned) {
            playerSpawn(players+i);
        }
    }
    
    // Init player maps
    for(int i=0; i<playerCount; i++) {
        initMiniMap(players+i);
    }
    
    stallCounter = 0;
}

void syncedTick() {
    int i;
    
    setListenerPosition(getLocalPlayer()->entity.level, getLocalPlayer()->entity.x, getLocalPlayer()->entity.y);

    //win/death menus
    for(i=0; i<playerCount; i++) {
        if (players[i].entity.p.isDead) {
            if (players[i].entity.p.endTimer < 1) {
                players[i].ingameMenu = MENU_LOSE;
            }
            --players[i].entity.p.endTimer;
        } else if (players[i].entity.p.hasWon) {
            if (players[i].entity.p.endTimer < 1) {
                players[i].ingameMenu = MENU_WIN;
            }
            --players[i].entity.p.endTimer;
        }
    }

    //update worldData (daytime,season and weather)
    ++worldData.daytime;
    if(worldData.daytime>=24000) {
        worldData.daytime -= 24000;
        ++worldData.day;
        //TODO: maybe make season length not as hardcoded + make the transition better (fade to black and back maybe?)
        if(worldData.day%7==0) {
            ++worldData.season;
            if(worldData.season==4) worldData.season = 0;
        }
        worldData.rain = false;
        if(worldData.season!=3 && rand()%5==0) worldData.rain = true;
    }
    
    //update music
    updateMusic(getLocalPlayer()->entity.level, worldData.daytime);
    
    //for every active level
    s8 level;
    for(level = 0; level < 6; level++) {
        bool hasPlayer = false;
        for(i=0; i<playerCount; i++) {
            if(players[i].entity.level==level) {
                hasPlayer = true;
            }
        }
        if(!hasPlayer) continue;

        //tick tiles
        for (i = 0; i < 324; ++i) {
            int xx = rand() & 127;
            int yy = rand() & 127;
            tickTile(level, xx, yy);
        }
    }
    
    
    for(i=0; i<playerCount; i++) {
        ingameMenuTickTouch(players+i);
        
        bool inmenu = false;
        if(players[i].ingameMenu != MENU_NONE) {
            ingameMenuTick(players+i, players[i].ingameMenu);
            inmenu = true;
        }
        
        tickPlayer(players+i, inmenu);
    }

    
    //for every active level
    for(level = 0; level < 6; level++) {
        if(level==5 && !dungeonActive()) continue;
        
        bool hasPlayer = false;
        for(i=0; i<playerCount; i++) {
            if(players[i].entity.level==level) {
                hasPlayer = true;
            }
        }
        if(!hasPlayer) continue;
        
        //spawn entities
        if(eManager.lastSlot[level]<80 && level != 5) {
            trySpawn(1, level);
        }
        
        //update entities
        for (i = 0; i < eManager.lastSlot[level]; ++i) {
            Entity * e = &eManager.entities[level][i];
            PlayerData * p = getNearestPlayer(level, e->x, e->y);
            
            //should never happen, but just for safety to prevent hard crashes
            if(p==NULL) continue;
            
            if ((e->type != ENTITY_ZOMBIE && e->type != ENTITY_SKELETON && e->type != ENTITY_KNIGHT && e->type != ENTITY_SLIME && e->type != ENTITY_PASSIVE && e->type != ENTITY_GLOWWORM) 
                || (e->type == ENTITY_GLOWWORM && (worldData.daytime>6000 || worldData.daytime<18000)) 
                || (e->x > p->entity.x - 160 && e->y > p->entity.y - 125 && e->x < p->entity.x + 160 && e->y < p->entity.y + 125)) {
                tickEntity(e);
            }
        }
    }
    
    stallCounter = 0;
}

void tickGame() {
    synchronizerTick(&syncedTick);
    
    
    if(synchronizerIsRunning()) {
        stallCounter++;
        //game stalled -> most likely a player disconnected -> present option to exit game
        if(stallCounter>=STALL_TIME) {
            if(stallCounter==STALL_TIME) stallAreYouSure = false;
            
            //scan local inputs, because synchronizer only updates them when not stalled
            hidScanInput();
            tickKeys(&localInputs, hidKeysHeld(), hidKeysDown());
            
            if (localInputs.k_accept.clicked) { 
                if(stallAreYouSure) {
                    //create backup save
                    if(playerLocalID==0) {
                        char backupSaveFileName[256+32];
                        backupSaveFileName[0] = '\0';
                        
                        strncat(backupSaveFileName, currentFileName, strlen(currentFileName)-4);
                        strcat(backupSaveFileName, ".exit.msv");
                        
                        saveWorld(backupSaveFileName, &eManager, &worldData, players, playerCount);
                    }
                    
                    exitGame();
                } else {
                    stallAreYouSure = true;
                }
            }
            if (localInputs.k_decline.clicked) { 
                stallAreYouSure = false;
            }
        }
    }
}

void renderGame() {
    //Important: all code called from this function should never affect game state!
    sf2d_start_frame(GFX_TOP, GFX_LEFT);

    //"camera" code -> move to own class (when adding new dungeon system)
    int xscr = getLocalPlayer()->entity.x - 100;
    int yscr = getLocalPlayer()->entity.y - 56;
    if (xscr < 16)
        xscr = 16;
    else if (xscr > 1832)
        xscr = 1832;
    if (yscr < 16)
        yscr = 16;
    else if (yscr > 1912)
        yscr = 1912;
    
    
    offsetX = xscr;
    offsetY = yscr;
    sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //RGBA8(12, 12, 12, 255)); //You might think "real" black would be better, but it actually looks better that way
    
    bool nightvision = playerEffectActive(getLocalPlayer(), EFFECT_NIGHTVISION);
    if(!nightvision) {
        renderLightsToStencil(getLocalPlayer(), false, false, true);
    }
    
    renderBackground(getLocalPlayer()->entity.level, xscr, yscr);
    renderEntities(getLocalPlayer()->entity.level, getLocalPlayer()->entity.x, getLocalPlayer()->entity.y, &eManager);
    for(int i=0; i<playerCount; i++) {
        renderPlayer(players+i);
    }
    renderWeather(getLocalPlayer()->entity.level, xscr, yscr);
    
    if(!nightvision) {
        resetStencil();
    }
    
    if(!nightvision) {
        renderDayNight(getLocalPlayer());
    }
    
    offsetX = 0;
    offsetY = 0;
    
    if(shouldRenderDebug){
        sprintf(fpsstr, " FPS: %.0f, X:%d, Y:%d, E:%d", sf2d_get_fps(), getLocalPlayer()->entity.x, getLocalPlayer()->entity.y, eManager.lastSlot[getLocalPlayer()->entity.level]);
        renderText(fpsstr, 2, 225);
    }
    
    if(getLocalPlayer()->ingameMenu != MENU_NONE) {
        ingameMenuRender(getLocalPlayer(), getLocalPlayer()->ingameMenu);
    }
    
    //game stalled -> most likely a player disconnected -> present option to exit game
    if(stallCounter>STALL_TIME) {
        renderFrame(1,1,24,14,0xFF1010AF);
        renderText("Waiting for a long time", (400 - (23 * 12))/2, 32);
        
        char text[50];
        sprintf(text, "Last response %is ago", stallCounter/60);
        renderText(text, (400 - (strlen(text) * 12))/2, 64);

        if(playerLocalID==0) {
            renderText("Press   to leave the game", (400 - (25 * 12))/2, 160);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 120, 157, 1);
            
            renderText("A backup save will be created", (400 - (29 * 12))/2, 192);
        } else {
            renderText("Press   to leave the game", (400 - (25 * 12))/2, 192);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 120, 189, 1);
        }
        
        if(stallAreYouSure){
            renderFrame(6,5,19,10,0xFF10108F);
            
            renderText("Are you sure?",122,96);
            renderText("   Yes", 164, 117);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 166, 114, 1);
            renderText("   No", 170, 133);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 166, 130, 1);
        }
    }
    
    sf2d_end_frame();

    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        if(!players[playerLocalID].mapShouldRender){
            sf2d_draw_texture(bottombg, 0, 0);
            renderGui(getLocalPlayer());
        } else {
            renderZoomedMap(getLocalPlayer());
        }
    sf2d_end_frame();
}

void exitGame() {
    networkDisconnect();
    synchronizerReset();
    
    sf2d_set_clear_color(0xFF);
    currentSelection = 0;
    currentMenu = MENU_TITLE;
    
    playMusic(&music_menu);
}
