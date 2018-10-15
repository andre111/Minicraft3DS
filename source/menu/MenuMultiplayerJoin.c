#include "MenuMultiplayerJoin.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "../network/Network.h"

int menuScanTimer = 0;

void menuMultiplayerJoinTick() {
    if(menuScanTimer>0) {
        menuScanTimer--;
    } else {
        networkScan();
        menuScanTimer = 30;
    }
    if(currentSelection >= networkGetScanCount()) currentSelection=networkGetScanCount()-1;
    if(currentSelection < 0) currentSelection = 0;

    if (localInputs.k_decline.clicked){
        currentMenu = MENU_CHOOSE_GAME;
        currentSelection = 2;
    }
    if (localInputs.k_up.clicked) { --currentSelection; if(currentSelection < 0) currentSelection = (networkGetScanCount()>0 ? networkGetScanCount()-1 : 0); }
    if (localInputs.k_down.clicked) { ++currentSelection; if(currentSelection >= networkGetScanCount()) currentSelection=0; }

    if(localInputs.k_accept.clicked) {
        if(networkGetScanCount()!=0) {
            for(int t=0; t<10; t++) { //try to connect multiple times, because it will not work the first try every time
                if(networkConnect(currentSelection)) {
                    currentMenu = MENU_MULTIPLAYER_WAIT;
                    currentSelection = 0;
                    break;
                }
            }
        }
    }
}

void menuMultiplayerJoinRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        offsetX = 0;offsetY = (currentSelection * 32) - 48;
        renderText("Select a world",122,-16);
        for(int i = 0; i < networkGetScanCount(); ++i){
            int color = 0xFF921020;
            char * text = malloc((50+8+1) * sizeof(char));
            memset(text, 0, (50+8+1) * sizeof(char));
            networkGetScanName(text, i);
            strcat(text, "'s World");
            
            if(i != currentSelection) color &= 0xFF7F7F7F; // Darken color.
            
            renderFrame(1,i*4,24,(i*4)+4,color);
            renderText(text,(400-(strlen(text)*12))/2,i*64+24);
            
            free(text);
        }
        offsetX = 0;offsetY = 0;
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        renderTextColor("Searching for Worlds",40,12,0xFF3FFFFF);
        renderText("Press   or   to scroll", 28, 50);
        renderButtonIcon(localInputs.k_up.input & -localInputs.k_up.input, 98, 48, 1);
        renderButtonIcon(localInputs.k_down.input & -localInputs.k_down.input, 160, 48, 1);
        renderText("Press   to join world", (320-21*12)/2, 100);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 104, 98, 1);
        renderText("Press   to return", 58, 150);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
    sf2d_end_frame();
}
