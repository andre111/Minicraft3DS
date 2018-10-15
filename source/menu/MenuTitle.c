#include "MenuTitle.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "MenuLoadGame.h"
#include "MenuTutorial.h"

char options[][12] = {"Start Game", "Editor", "How To Play", "Settings", "About", "Exit"};

void menuTitleTick() {
    menuUpdateMapBG();

    if (localInputs.k_up.clicked) { --currentSelection; if(currentSelection < 0) currentSelection=5; }
    if (localInputs.k_down.clicked) { ++currentSelection; if(currentSelection > 5) currentSelection=0; }
    
    if(localInputs.k_accept.clicked) {
        switch(currentSelection) {
            case 0:
                currentSelection = 0;
                currentMenu = MENU_CHOOSE_GAME;
            break;
            case 1:
                currentSelection = 0;
                currentMenu = MENU_LOADGAME;
                menuLoadGameInit(LOAD_TO_EDITOR);
            break;
            case 2:
                currentMenu = MENU_TUTORIAL;
                menuTutorialInit();
            break;
            case 3:
                currentSelection = 0;
                currentMenu = MENU_SETTINGS;
            break;
            case 4: 
                currentMenu = MENU_ABOUT;
            break;
            case 5: 
                quitGame = true;
            break;
        }
    }
}

void menuTitleRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        menuRenderMapBGTop();
    
        renderTitle(76, 12);
    
        for(int i = 5; i >= 0; --i) {
            char* msg = options[i];
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
    
        int startX = 0, startY = 0;// relative coordinates ftw
        switch(currentSelection) {
            case 0: // "Start Game"
            
            break;
            case 1: // "Editor"
            
            break;
            case 2: // "How To Play"
                startX = 72; startY = 54;
                render16(startX, startY, 96, 208, 0);//C-PAD
                startX = 72; startY = 37;
                render16(startX, startY-16, 16, 112, 0);//Player
                render16(startX, startY, 112, 208, 0);//C-PAD up
                startX = 72; startY = 71;
                render16(startX, startY+16, 0, 112, 0);//Player
                render16(startX, startY, 144, 208, 0);//C-PAD down
                startX = 39; startY = 54;
                render16(startX, startY, 48, 112, 1);//Player
                render16(startX+16, startY, 128, 208, 0);//C-PAD left
                startX = 89; startY = 54;
                render16(startX+16, startY, 48, 112, 0);//Player
                render16(startX, startY, 160, 208, 0);//C-PAD right
                renderTextColor("Learn the basics", 64, 24, 0xFF7FFFFF);
            break;
            case 3: // "Settings"
                renderTextColor("Modify the game's feel", (320 - (22 * 12))/2, 24, 0xFF7FFFFF);
                renderc(48,48,0,112,64,32,0);
            break;
            case 4: // "About"
                renderTextColor("Who made this game?", (320 - (19 * 12))/2, 24, 0xFF7FFFFF);
            break;
            case 5: // "Exit"
                renderTextColor("Exit to the home menu", (320 - (21 * 12))/2, 24, 0xFF7FFFFF);
                renderTextColor("(bye-bye)", (320 - (9 * 12))/2, 100, 0xFF7FFFFF);
            break;
        }
    sf2d_end_frame();
}
