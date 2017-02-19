#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <string.h> 
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#include "Globals.h"
#include "Render.h"
#include "MapGen.h"
#include "Menu.h"
#include "texturepack.h"
#include "Network.h"

// TODO: Dungeon is way to difficult
//       -> Skeleton arrows are slower, do a little less damage
//       -> Or instead of less damage, implement a simple armor system

void initMiniMapData() {
	int i;
	for(i = 0; i < 128 * 128; ++i) {
		minimapData[i] = 0;
	}
}

void initMiniMap(bool loadUpWorld) {
	int i;
	for (i = 0; i < 5; ++i) {
		initMinimapLevel(i, loadUpWorld);
	}
}

void initNewMap() {
	newSeed();
	createAndValidateSkyMap(128, 128, 0, map[0], data[0]);
	createAndValidateTopMap(128, 128, 1, map[1], data[1]);
	createAndValidateUndergroundMap(128, 128, 1, 2, map[2], data[2]);
	createAndValidateUndergroundMap(128, 128, 2, 3, map[3], data[3]);
	createAndValidateUndergroundMap(128, 128, 3, 4, map[4], data[4]);
}

void setupGame(bool loadUpWorld, bool remote) {
	currentLevel = 1;

	// Reset entity manager.
	memset(&eManager, 0, sizeof(eManager));
	sf2d_set_clear_color(0xFF6C6D82); //sf2d_set_clear_color(RGBA8(0x82, 0x6D, 0x6C, 0xFF));

	initMiniMapData();
	
    if(!remote) {
        if (!loadUpWorld) {
            initNewMap();
            initPlayer();
            resetQuests();
            airWizardHealthDisplay = 2000;
            int i;
            for (i = 0; i < 5; ++i) {
                trySpawn(500, i);
            }
            addEntityToList(newAirWizardEntity(630, 820, 0), &eManager);
            daytime = 6000;
            day = 0;
            season = 0;
            rain = false;
        } else {
            initPlayer();
            resetQuests();
            loadWorld(currentFileName, &eManager, &player, (u8*) map, (u8*) data);
        }
        
        updateMusic(currentLevel, daytime);

        initMiniMap(loadUpWorld);
    } else {
        //TODO: Can Packets get dropped - if yes, should resending be handled by network functions (so I dont need to do it everywhere)
        networkPacket packet = {
            .requestMapData = {
                .type = PACKET_REQUEST_MAPDATA,
                .level = 1
            }
        };
        networkSend(&packet, sizeof(packetRequestMapData));
    }
    
	shouldRenderMap = false;
	mScrollX = 0;
	mScrollY = 0;
	zoomLevel = 2;
    sprintf(mapText,"x%d",zoomLevel);
	initGame = 0;
}

void setupBGMap(bool loadUpWorld) {
	// Reset entity manager.
	memset(&eManager, 0, sizeof(eManager));
	sf2d_set_clear_color(0xFF6C6D82); //sf2d_set_clear_color(RGBA8(0x82, 0x6D, 0x6C, 0xFF));
	
	if(!loadUpWorld) {
		newSeed();
		createAndValidateTopMap(128, 128, 1, map[1], data[1]);
	} else {
		loadWorld(currentFileName, &eManager, &player, (u8*) map, (u8*) data);
	}
	
	// Reset entity manager.
	memset(&eManager, 0, sizeof(eManager));
	sf2d_set_clear_color(0xFF6C6D82); //sf2d_set_clear_color(RGBA8(0x82, 0x6D, 0x6C, 0xFF));
	
	initBGMap = 0;
}

int xscr = 0, yscr = 0;
void tick() {
    //TODO: Some stuff DOES need to happen even on client side
    if(!isRemote) {
        if (player.p.isDead) {
            if (player.p.endTimer < 1) {
                currentMenu = MENU_LOSE;
            }
            --player.p.endTimer;
            return;
        } else if (player.p.hasWon) {
            if (player.p.endTimer < 1) {
                currentMenu = MENU_WIN;
            }
            --player.p.endTimer;
            return;
        }
        
        tickTouchMap();
        tickTouchQuickSelect();

        ++daytime;
        //daytime += 20;
        if(daytime>=24000) {
            daytime -= 24000;
            ++day;
            //TODO: maybe make season length not as hardcoded + make the transition better (fade to black and back maybe?)
            if(day%7==0) {
                ++season;
                if(season==4) season = 0;
            }
            rain = false;
            if(season!=3 && rand()%5==0) rain = true;
        }
        if(daytime==6000 && currentLevel==1) {
            playMusic(music_floor1);
        } else if(daytime==19000 && currentLevel==1) {
            playMusic(music_floor1_night);
        }
        
        int i;
        for (i = 0; i < 324; ++i) {
            int xx = rand() & 127;
            int yy = rand() & 127;
            tickTile(xx, yy);
        }
        tickPlayer();
        xscr = player.x - 100;
        yscr = player.y - 56;
        if (xscr < 16)
            xscr = 16;
        else if (xscr > 1832)
            xscr = 1832;
        if (yscr < 16)
            yscr = 16;
        else if (yscr > 1912)
            yscr = 1912;
        
        if(eManager.lastSlot[currentLevel]<80 && currentLevel != 5) {
            trySpawn(1, currentLevel);
        }

        for (i = 0; i < eManager.lastSlot[currentLevel]; ++i) {
            Entity * e = &eManager.entities[currentLevel][i];
            if ((e->type != ENTITY_ZOMBIE && e->type != ENTITY_SKELETON && e->type != ENTITY_KNIGHT && e->type != ENTITY_SLIME && e->type != ENTITY_PASSIVE) 
                || (e->type == ENTITY_GLOWWORM && (daytime>6000 || daytime<18000)) 
                || (e->x > player.x - 160 && e->y > player.y - 125 && e->x < player.x + 160 && e->y < player.y + 125))
                tickEntity(e);
        }
    }
}

char debugText[34];
char bossHealthText[34];
int main() {
    cfguInit();
    CFGU_GetSystemModel(&MODEL_3DS);
	FILE * file;
	shouldRenderDebug = true;
	if ((file = fopen("settings.bin", "r"))) {
        fread(&shouldRenderDebug,sizeof(bool),1,file);
        fread(&shouldSpeedup,sizeof(bool),1,file);
        osSetSpeedupEnable(shouldSpeedup);
		fclose(file);
	}
    sf2d_init();
	csndInit();
	networkInit();
	
	noItem = newItem(ITEM_NULL, 0);
	
	initMenus();
	currentMenu = MENU_TITLE;
	currentSelection = 0;
	quitGame = false;

	icons = sfil_load_PNG_buffer(icons2_png, SF2D_PLACE_RAM);
	font = sfil_load_PNG_buffer(Font_png, SF2D_PLACE_RAM);
	bottombg = sfil_load_PNG_buffer(bottombg_png, SF2D_PLACE_RAM);

	loadSounds();
	playMusic(music_menu);
	
	bakeLights();
	

	int i;
	for (i = 0; i < 6; ++i) {
		minimap[i] = sf2d_create_texture(128, 128, TEXFMT_RGBA8, SF2D_PLACE_RAM);
		sf2d_texture_tile32(minimap[i]);
	}
	
	reloadColors();
	
	sf2d_set_vblank_wait(true);

	sf2d_set_clear_color(0xFF);

	/* Default inputs */
	k_up.input = KEY_DUP | KEY_CPAD_UP | KEY_CSTICK_UP;
	k_down.input = KEY_DDOWN | KEY_CPAD_DOWN | KEY_CSTICK_DOWN;
	k_left.input = KEY_DLEFT | KEY_CPAD_LEFT | KEY_CSTICK_LEFT;
	k_right.input = KEY_DRIGHT | KEY_CPAD_RIGHT | KEY_CSTICK_RIGHT;
	k_attack.input = KEY_A | KEY_B | KEY_L | KEY_ZR;
	k_menu.input = KEY_X | KEY_Y | KEY_R | KEY_ZL;
	k_pause.input = KEY_START;
	k_accept.input = KEY_A;
	k_decline.input = KEY_B;
	k_delete.input = KEY_X;
	k_menuNext.input = KEY_R;
	k_menuPrev.input = KEY_L;

	/* If btnSave exists, then use that. */
	if ((file = fopen("btnSave.bin", "rb"))) {
		fread(&k_up.input, sizeof(int), 1, file);
		fread(&k_down.input, sizeof(int), 1, file);
		fread(&k_left.input, sizeof(int), 1, file);
		fread(&k_right.input, sizeof(int), 1, file);
		fread(&k_attack.input, sizeof(int), 1, file);
		fread(&k_menu.input, sizeof(int), 1, file);
		fread(&k_pause.input, sizeof(int), 1, file);
		fread(&k_accept.input, sizeof(int), 1, file);
		fread(&k_decline.input, sizeof(int), 1, file);
		fread(&k_delete.input, sizeof(int), 1, file);
		fread(&k_menuNext.input, sizeof(int), 1, file);
		fread(&k_menuPrev.input, sizeof(int), 1, file);
		fclose(file);
	}
	
	/* If lastTP exists, then use that. */
	if ((file = fopen("lastTP.bin", "r"))) {
		char fnbuf[256];
		fgets(fnbuf, 256, file); // get directory to texturepack
		loadTexturePack(fnbuf);   
		fclose(file);
	}

	tickCount = 0;
	initRecipes();
    initQuests();
	while (aptMainLoop()) {
		++tickCount;
		hidScanInput();
		tickKeys(hidKeysHeld(), hidKeysDown());

		if (quitGame) break;

		if (initGame > 0) setupGame(initGame == 1 ? true : false, isRemote);
		if (initBGMap > 0) setupBGMap(initBGMap == 1 ? true : false);

        networkRecieve();
        
		if (currentMenu == 0) {
			tick();
			sf2d_start_frame(GFX_TOP, GFX_LEFT);

			offsetX = xscr;
			offsetY = yscr;
			sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //RGBA8(12, 12, 12, 255)); //You might think "real" black would be better, but it actually looks better that way
			
			renderLightsToStencil(false, false, true);

			renderBackground(xscr, yscr);
			renderEntities(player.x, player.y, &eManager);
			renderPlayer();
            renderWeather(xscr, yscr);
			
			resetStencilStuff();
			
			renderDayNight();
			
			offsetX = 0;
			offsetY = 0;
			
			if(shouldRenderDebug){
			    sprintf(fpsstr, " FPS: %.0f, X:%d, Y:%d, E:%d", sf2d_get_fps(), player.x, player.y, eManager.lastSlot[currentLevel]);
			    drawText(fpsstr, 2, 225);
            }
			
			sf2d_end_frame();

            sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
                if(!shouldRenderMap){
                    sf2d_draw_texture(bottombg, 0, 0);
                    renderGui();
                } else {
                    renderZoomedMap();
                }
            sf2d_end_frame();
		} else {
			tickMenu(currentMenu);
			renderMenu(currentMenu, xscr, yscr);
		}

		sf2d_swapbuffers();
	}
	
	stopMusic();

    freeQuests();
	freeRecipes();

	freeLightBakes();
	sf2d_free_texture(icons);
	sf2d_free_texture(minimap[0]);
	sf2d_free_texture(minimap[1]);
	sf2d_free_texture(minimap[2]);
	sf2d_free_texture(minimap[3]);
	sf2d_free_texture(minimap[4]);
	sf2d_free_texture(minimap[5]);
	freeSounds();
	networkExit();
	csndExit();
    cfguExit();
	sf2d_fini();
	return 0;
}
