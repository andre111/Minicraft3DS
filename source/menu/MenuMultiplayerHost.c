#include "MenuMultiplayerHost.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "../network/Network.h"

#include "MenuLoadGame.h"

void menuMultiplayerHostTick() {
    if (localInputs.k_decline.clicked){
        networkDisconnect();
        
        currentMenu = MENU_CHOOSE_GAME;
        currentSelection = 1;
    }
    
    if(localInputs.k_accept.clicked){
        if(networkGetNodeCount()>1) {
            currentMenu = MENU_LOADGAME;
            menuLoadGameInit(LOAD_TO_MULTIPLAYER);
            currentSelection = 0;
        }
    }
}

void menuMultiplayerHostRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        networkUpdateStatus();
        renderText("Connected Players",98,8);
        int j = 0;
        int lastj = 0;
        for(int i = 0; i<networkGetNodeCount(); i++) {
            for(j = lastj+1; j <= UDS_MAXNODES; j++) {
                if(networkIsNodeConnected(j)) {
                    char * text = malloc((50+8+1) * sizeof(char));
                    memset(text, 0, (50+8+1) * sizeof(char));
                    networkGetNodeName(j, text);
                    
                    renderText(text,(400-(strlen(text)*12))/2,i*26+32);
                    
                    free(text);
                    lastj = j;
                    break;
                }
            }
        }
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        renderText("Press   to start game", (320-21*12)/2, 100);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 104, 98, 1);
        renderText("Press   to return", 58, 150);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
    sf2d_end_frame();
}
