#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "Globals.h"
#include "Data.h"
#include "Render.h"
#include "render/Batching.h"
#include "MapGen.h"
#include "Menu.h"
#include "texturepack.h"
#include "SaveLoad.h"
#include "Ingame.h"
#include "Player.h"
#include "network/Network.h"
#include "network/PacketHandler.h"
#include "network/Synchronizer.h"

// TODO: Dungeon is way to difficult
//       -> Skeleton arrows are slower, do a little less damage
//       -> Or instead of less damage, implement a simple armor system

//TODO: Something still causes desyncs very rarely

//TODO: More Batched Rendering (most importantly ingame menus)
//TODO: IMPORTANT: Batching causes flickering rendering (especially when performance is low/on o3ds), sometimes it even hangs up the main thread?

void setupGame() {
    synchronizerInit(rand(), 1, 0);
    synchronizerSetPlayerUID(0, localUID);
    synchronizerStart();
    
    initGame = 0;
}

void setupGameServer() {
    size_t size;
    
    networkHostStopConnections();
    
    networkStart();
    
    //send start info (seed)
    size = writeStartPacket(networkWriteBuffer, rand());
    networkSend(networkWriteBuffer, size);
    processPacket(networkWriteBuffer, size);
    networkSendWaitFlush();
    
    //send save file if loading
    FILE *file = fopen(currentFileName, "rb");
    if(file!=NULL) {
        sendFile(file, 0, 0);
        networkSendWaitFlush();
        fclose(file);
    }
    
    //send start command
    size = writeStartRequestPacket(networkWriteBuffer);
    networkSend(networkWriteBuffer, size);
    processPacket(networkWriteBuffer, size);
    
    initMPGame = 0;
}

void setupBGMap() {
    // Reset entity manager.
    memset(&eManager, 0, sizeof(eManager));
    sf2d_set_clear_color(0xFF6C6D82);
    

    srand(time(NULL));
    createAndValidateTopMap(128, 128, 1, worldData.map[1], worldData.data[1]);
    
    // Reset entity manager.
    memset(&eManager, 0, sizeof(eManager));
    sf2d_set_clear_color(0xFF6C6D82);
    
    initBGMap = 0;
}



char debugText[34];
char bossHealthText[34];
int main() {
    cfguInit();
    CFGU_GetSystemModel(&MODEL_3DS);
    FILE * file;
    shouldRenderDebug = false;
    if ((file = fopen("settings.bin", "r"))) {
        fread(&shouldRenderDebug,sizeof(bool),1,file);
        fread(&shouldSpeedup,sizeof(bool),1,file);
        osSetSpeedupEnable(shouldSpeedup);
        fclose(file);
    }
    sf2d_init();
    csndInit();
    networkInit();
    romfsInit();
    
    srand(time(NULL));
    
    //load or create localUID
    if ((file = fopen("m3ds_uid.bin", "rb"))) {
        fread(&localUID, sizeof(u32), 1, file);
        fclose(file);
    } else {
        localUID = (((u32) (rand()%256))<<24) | (((u32) (rand()%256))<<16) | (((u32) (rand()%256))<<8) | (((u32) (rand()%256)));
        
        if ((file = fopen("m3ds_uid.bin", "wb"))) {
            fwrite(&localUID, sizeof(u32), 1, file);
            fclose(file);
        }
    }
    
    noItem = newItem(ITEM_NULL, 0);
    
    initMenus();
    currentMenu = MENU_TITLE;
    currentSelection = 0;
    quitGame = false;
    initBGMap = 1;

    icons = sfil_load_PNG_buffer(icons_png, SF2D_PLACE_RAM);
    playerSprites = sfil_load_PNG_buffer(player_png, SF2D_PLACE_RAM);
    font = sfil_load_PNG_buffer(Font_png, SF2D_PLACE_RAM);
    bottombg = sfil_load_PNG_buffer(bottombg_png, SF2D_PLACE_RAM);
    
    batch_init(1024*10);

    loadSounds();
    playMusic(&music_menu);
    
    bakeLights();
    

    int i;
    for (i = 0; i < 6; ++i) {
        minimap[i] = sf2d_create_texture(128, 128, TEXFMT_RGBA8, SF2D_PLACE_RAM);
        sf2d_texture_tile32(minimap[i]);
    }
    
    reloadColors();
    
    sf2d_set_vblank_wait(true);

    sf2d_set_clear_color(0xFF);

    /* Default inputs (also located in MenuSettingsRebind.c) */
    localInputs.k_up.input = KEY_DUP | KEY_CPAD_UP | KEY_CSTICK_UP;
    localInputs.k_down.input = KEY_DDOWN | KEY_CPAD_DOWN | KEY_CSTICK_DOWN;
    localInputs.k_left.input = KEY_DLEFT | KEY_CPAD_LEFT | KEY_CSTICK_LEFT;
    localInputs.k_right.input = KEY_DRIGHT | KEY_CPAD_RIGHT | KEY_CSTICK_RIGHT;
    localInputs.k_attack.input = KEY_A | KEY_B | KEY_L | KEY_ZR;
    localInputs.k_menu.input = KEY_X | KEY_Y | KEY_R | KEY_ZL;
    localInputs.k_pause.input = KEY_START;
    localInputs.k_accept.input = KEY_A;
    localInputs.k_decline.input = KEY_B;
    localInputs.k_delete.input = KEY_X;
    localInputs.k_menuNext.input = KEY_R;
    localInputs.k_menuPrev.input = KEY_L;

    /* If btnSave exists, then use that. */
    if ((file = fopen("btnSave.bin", "rb"))) {
        fread(&(localInputs.k_up.input), sizeof(int), 1, file);
        fread(&(localInputs.k_down.input), sizeof(int), 1, file);
        fread(&(localInputs.k_left.input), sizeof(int), 1, file);
        fread(&(localInputs.k_right.input), sizeof(int), 1, file);
        fread(&(localInputs.k_attack.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menu.input), sizeof(int), 1, file);
        fread(&(localInputs.k_pause.input), sizeof(int), 1, file);
        fread(&(localInputs.k_accept.input), sizeof(int), 1, file);
        fread(&(localInputs.k_decline.input), sizeof(int), 1, file);
        fread(&(localInputs.k_delete.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menuNext.input), sizeof(int), 1, file);
        fread(&(localInputs.k_menuPrev.input), sizeof(int), 1, file);
        fclose(file);
    }
    
    /* If lastTP exists, then use that. */
    if ((file = fopen("lastTP.bin", "r"))) {
        char fnbuf[256];
        fgets(fnbuf, 256, file); // get directory to texturepack
        loadTexturePack(fnbuf);   
        fclose(file);
    }
    
    initData();
    
    initPlayers();
    initRecipes();
    initTrades();
    while (aptMainLoop()) {
        if (quitGame) break;
        
        batch_reset();

        if (initGame > 0 && --initGame==0) setupGame();
        if (initMPGame > 0 && --initMPGame==0) setupGameServer();
        if (initBGMap > 0 && --initBGMap==0) setupBGMap();
        
        if (currentMenu == MENU_NONE) {
            tickGame();
            renderGame();
        } else {
            //input scanning ingame is handled by the synchronizer
            hidScanInput();
            tickKeys(&localInputs, hidKeysHeld(), hidKeysDown());
            
            tickMenu(currentMenu);
            renderMenu(currentMenu);
        }

        sf2d_swapbuffers();
    }
    
    stopMusic();

    freeTrades();
    freeRecipes();
    freePlayers();

    freeLightBakes();
    sf2d_free_texture(icons);
    sf2d_free_texture(minimap[0]);
    sf2d_free_texture(minimap[1]);
    sf2d_free_texture(minimap[2]);
    sf2d_free_texture(minimap[3]);
    sf2d_free_texture(minimap[4]);
    sf2d_free_texture(minimap[5]);
    freeSounds();
    
    batch_free();
    
    romfsExit();
    networkExit();
    csndExit();
    cfguExit();
    sf2d_fini();
    return 0;
}
