#include "MenuChooseGame.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "../network/Network.h"

#include "MenuLoadGame.h"

char gameOptions[][24] = {"Singleplayer", "Host Game", "Join Game", "Back"};

void menuChooseGameTick() {
    menuUpdateMapBG();

    if (localInputs.k_up.clicked) { --currentSelection; if(currentSelection < 0) currentSelection=3; }
    if (localInputs.k_down.clicked) { ++currentSelection; if(currentSelection > 3) currentSelection=0; }
    
    if(localInputs.k_accept.clicked) {
        switch(currentSelection) {
            case 0:
                currentMenu = MENU_LOADGAME;
                menuLoadGameInit(LOAD_TO_SINGLEPLAYER);
                currentSelection = 0;
            break;
            case 1:
                if(networkHost()) {
                    currentMenu = MENU_MULTIPLAYER_HOST;
                    currentSelection = 0;
                }
            break;
            case 2:
                networkScan();
                currentMenu = MENU_MULTIPLAYER_JOIN;
                currentSelection = 0;
            break;
            case 3:
                currentMenu = MENU_TITLE;
                currentSelection = 0;
            break;
        }
    }
    if(localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
        currentSelection = 0;
    }
}

void menuChooseGameRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        menuRenderMapBGTop();
    
        renderTitle(76, 12);
    
        for(int i = 3; i >= 0; --i) {
            char* msg = gameOptions[i];
            u32 color = 0xFF7F7F7F;
            if(i == currentSelection) color = 0xFFFFFFFF;
            renderTextColorSized(msg,((200 - (strlen(msg) * 8))/2) + 1, (((8 + i) * 20 - 66) >> 1) + 1,2.0, 0xFF000000);   
            renderTextColorSized(msg,(200 - (strlen(msg) * 8))/2, ((8 + i) * 20 - 66) >> 1,2.0, color);    
        }
        
        renderText(versionText,2,225);
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        menuRenderMapBGBottom();
        
        switch(currentSelection) {
            case 0: // "Start Game"
            
            break;
            case 1: // "Host Game"
                renderTextColor("Host local multiplayer", (320 - (22 * 12))/2, 24, 0xFF7FFFFF);
            break;
            case 2: // "Join Game"
                renderTextColor("Join local multiplayer", (320 - (22 * 12))/2, 24, 0xFF7FFFFF);
            break;
            case 3: // "Back"
                renderTextColor("Return to title screen", (320 - (22 * 12))/2, 24, 0xFF7FFFFF);
            break;
        }
    sf2d_end_frame();
}
