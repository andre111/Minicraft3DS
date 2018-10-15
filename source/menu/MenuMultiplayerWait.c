#include "MenuMultiplayerWait.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "../network/Network.h"

void menuMultiplayerWaitTick() {
    if (localInputs.k_decline.clicked){
        networkDisconnect();
        
        currentMenu = MENU_TITLE;
        currentSelection = 2;
    }
}

void menuMultiplayerWaitRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        networkUpdateStatus();
        renderText("Connected to",(400-12*12)/2,1*26+32);
        if(networkIsNodeConnected(1)) {
            char * text = malloc((50+8+1) * sizeof(char));
            memset(text, 0, (50+8+1) * sizeof(char));
            networkGetNodeName(1, text);
            
            renderText(text,(400-(strlen(text)*12))/2,2*26+32);
            
            free(text);
        }
        renderText("Waiting for host to start",(400-25*12)/2,3*26+32);
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        renderText("Awaiting game start..", (320-21*12)/2, 100);
        renderText("Press   to return", 58, 150);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
    sf2d_end_frame();
}
