#include "Menu.h"

#include "Render.h"
#include "editor/Editor.h"

void initMenus() {
    menuInitMapBG();
}

void tickMenu(int menu){
    switch(menu) {
        case MENU_TITLE:
            menuTitleTick();
        break;
        case MENU_CHOOSE_GAME:
            menuChooseGameTick();
        break;
        case MENU_LOADGAME:
            menuLoadGameTick();
        break;
        case MENU_MULTIPLAYER_HOST:
            menuMultiplayerHostTick();
        break;
        case MENU_MULTIPLAYER_JOIN:
            menuMultiplayerJoinTick();
        break;
        case MENU_MULTIPLAYER_WAIT:
            menuMultiplayerWaitTick();
        break;
        case MENU_LOADING:
            menuLoadingTick();
        break;
        case MENU_EDITOR:
            editorTick();
        break;
        case MENU_TUTORIAL:
            menuTutorialTick();
        break;
        case MENU_SETTINGS:
            menuSettingsTick();
        break;
        case MENU_SETTINGS_REBIND:
            menuSettingsRebindTick();
        break;
        case MENU_SETTINGS_TP:
            menuSettingsTextureTick();
        break;
        case MENU_ABOUT:
            menuAboutTick();
        break;
    }
}

void renderMenu(int menu) {
    switch(menu) {
        case MENU_TITLE:
            menuTitleRender();
        break;
        case MENU_CHOOSE_GAME:
            menuChooseGameRender();
        break;
        case MENU_LOADGAME:
            menuLoadGameRender();
        break;
        case MENU_MULTIPLAYER_HOST:
            menuMultiplayerHostRender();
        break;
        case MENU_MULTIPLAYER_JOIN:
            menuMultiplayerJoinRender();
        break;
        case MENU_MULTIPLAYER_WAIT:
            menuMultiplayerWaitRender();
        break;
        case MENU_LOADING:
            menuLoadingRender();
        break;
        case MENU_EDITOR:
            editorRender();
        break;
        case MENU_TUTORIAL:
            menuTutorialRender();
        break;
        case MENU_SETTINGS:
            menuSettingsRender();
        break;
        case MENU_SETTINGS_REBIND:
            menuSettingsRebindRender();
        break;
        case MENU_SETTINGS_TP:
            menuSettingsTextureRender();
        break;
        case MENU_ABOUT:
            menuAboutRender();
        break;
    }

}



// Keyboard
char guiText0[] = "1 2 3 4 5 6 7 8 9 0";
char guiText1[] = "Q W E R T Y U I O P";
char guiText2[] = "A S D F G H J K L";
char guiText3[] = "Z X C V B N M";
char guiText4[] = " SPACE  BACKSPACE";

s8 touchDelay = 0;
bool isTouching = false;
int touchX = 0, touchY = 0, touchW = 0, touchH = 0;

void menuTickKeyboard(char * string, int maxLength) {
    if((localInputs.k_touch.px != 0 || localInputs.k_touch.py != 0) && touchDelay == 0) { 
        if(!isTouching) {
            int xVal = localInputs.k_touch.px, yVal = localInputs.k_touch.py;
            int strLength = strlen(string);
            if(yVal >= 60 && yVal < 80) { // 0 to 9
                if(xVal >= 4 && xVal < 4+16) { touchX = 4; if(strLength < maxLength) strncat(string, "1", 1); }
                else if(xVal >= 36 && xVal < 4+32+16) { touchX = 36; if(strLength < maxLength) strncat(string, "2", 1); }
                else if(xVal >= 68 && xVal < 4+64+16) { touchX = 68; if(strLength < maxLength) strncat(string, "3", 1); }
                else if(xVal >= 100 && xVal < 4+96+16) { touchX = 100; if(strLength < maxLength) strncat(string, "4", 1); }
                else if(xVal >= 132 && xVal < 4+128+16) { touchX = 132; if(strLength < maxLength) strncat(string, "5", 1); }
                else if(xVal >= 164 && xVal < 4+160+16) { touchX = 164; if(strLength < maxLength) strncat(string, "6", 1); }
                else if(xVal >= 196 && xVal < 4+192+16) { touchX = 196; if(strLength < maxLength) strncat(string, "7", 1); }
                else if(xVal >= 228 && xVal < 4+224+16) { touchX = 228; if(strLength < maxLength) strncat(string, "8", 1); }
                else if(xVal >= 260 && xVal < 4+256+16) { touchX = 260; if(strLength < maxLength) strncat(string, "9", 1); }
                else if(xVal >= 292 && xVal < 4+288+16) { touchX = 292; if(strLength < maxLength) strncat(string, "0", 1); }
                else return;
                touchY = 56;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if(yVal >= 80 && yVal < 100) { // Q to P
                if(xVal >= 4 && xVal < 4+16) { touchX = 4; if(strLength < maxLength) strncat(string, "Q", 1); }
                else if(xVal >= 36 && xVal < 4+32+16) { touchX = 36; if(strLength < maxLength) strncat(string, "W", 1); }
                else if(xVal >= 68 && xVal < 4+64+16) { touchX = 68; if(strLength < maxLength) strncat(string, "E", 1); }
                else if(xVal >= 100 && xVal < 4+96+16) { touchX = 100; if(strLength < maxLength) strncat(string, "R", 1); }
                else if(xVal >= 132 && xVal < 4+128+16) { touchX = 132; if(strLength < maxLength) strncat(string, "T", 1); }
                else if(xVal >= 164 && xVal < 4+160+16) { touchX = 164; if(strLength < maxLength) strncat(string, "Y", 1); }
                else if(xVal >= 196 && xVal < 4+192+16) { touchX = 196; if(strLength < maxLength) strncat(string, "U", 1); }
                else if(xVal >= 228 && xVal < 4+224+16) { touchX = 228; if(strLength < maxLength) strncat(string, "I", 1); }
                else if(xVal >= 260 && xVal < 4+256+16) { touchX = 260; if(strLength < maxLength) strncat(string, "O", 1); }
                else if(xVal >= 292 && xVal < 4+288+16) { touchX = 292; if(strLength < maxLength) strncat(string, "P", 1); }
                else return;
                touchY = 76;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if(yVal >= 100 && yVal < 120) { // A to L
                if(xVal >= 20 && xVal < 36) { touchX = 20; if(strLength < maxLength) strncat(string, "A", 1); }
                else if(xVal >= 52 && xVal < 68) { touchX = 52; if(strLength < maxLength) strncat(string, "S", 1); }
                else if(xVal >= 84 && xVal < 100) { touchX = 84; if(strLength < maxLength) strncat(string, "D", 1); }
                else if(xVal >= 116 && xVal < 132) { touchX = 116; if(strLength < maxLength) strncat(string, "F", 1); }
                else if(xVal >= 148 && xVal < 164) { touchX = 148; if(strLength < maxLength) strncat(string, "G", 1); }
                else if(xVal >= 180 && xVal < 196) { touchX = 180; if(strLength < maxLength) strncat(string, "H", 1); }
                else if(xVal >= 212 && xVal < 230) { touchX = 212; if(strLength < maxLength) strncat(string, "J", 1); }
                else if(xVal >= 244 && xVal < 262) { touchX = 244; if(strLength < maxLength) strncat(string, "K", 1); }
                else if(xVal >= 276 && xVal < 294) { touchX = 276; if(strLength < maxLength) strncat(string, "L", 1); }
                else return;
                touchY = 96;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if(yVal >= 120 && yVal < 140) { // Z to M
                if(xVal >= 52 && xVal < 68) { touchX = 52; if(strLength < maxLength) strncat(string, "Z", 1); }
                else if(xVal >= 84 && xVal < 100) { touchX = 84; if(strLength < maxLength) strncat(string, "X", 1); }
                else if(xVal >= 116 && xVal < 132) { touchX = 116; if(strLength < maxLength) strncat(string, "C", 1); }
                else if(xVal >= 148 && xVal < 164) { touchX = 148; if(strLength < maxLength) strncat(string, "V", 1); }
                else if(xVal >= 180 && xVal < 196) { touchX = 180; if(strLength < maxLength) strncat(string, "B", 1); }
                else if(xVal >= 212 && xVal < 230) { touchX = 212; if(strLength < maxLength) strncat(string, "N", 1); }
                else if(xVal >= 244 && xVal < 262) { touchX = 244; if(strLength < maxLength) strncat(string, "M", 1); }
                else return;
                touchY = 116;
                touchW = 22;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            } else if(yVal >= 140 && yVal < 160) { // SPACE and BACKSPACE
                if(xVal >= 32 && xVal < 112) { 
                    touchX = 32; 
                    touchW = 16*5+14;
                    if(strLength < maxLength) strncat(string, "_", 1); // Underscores don't appear in the game.
                } else if(xVal >= 148 && xVal < 262) { 
                    if(strLength <= 0) return;
                    touchX = 148; 
                    touchW = 16*9+8;
                    string[strLength-1] = '\0';
                } else {
                    return;
                }
                touchY = 136;
                touchH = 22;
                touchDelay = 6;
                isTouching = true;
            }
        }
    } else if(localInputs.k_touch.px == 0 || localInputs.k_touch.py == 0) {
        isTouching = false;
    }
    if(touchDelay > 0) --touchDelay;
}

void menuRenderKeyboard() {
    renderTextColor("Touch the keypad below",(320-22*12)/2,12,0xFF33FFFF);
            
    sf2d_draw_rectangle(0, 50, 320, 110, 0xFF7F7FBF);
    
    if(touchDelay > 0){
        sf2d_draw_rectangle(touchX, touchY, touchW, touchH, 0xFF0000AF);
    }
    
    renderTextSized(guiText0,4, 60, 2);
    renderTextSized(guiText1,4, 80, 2);
    renderTextSized(guiText2,12, 100, 2);
    renderTextSized(guiText3,28, 120, 2);
    renderTextSized(guiText4,12, 140, 2);
}



//Menu Map Background
bool menuHasMapLoaded = false;
float mxscr = 400;
float myscr = 400;
float menuxa = 0;
float menuya = 0;

void menuInitMapBG() {
    menuHasMapLoaded = true;
    while(menuxa==0) menuxa = (rand()%3 - 1) * 0.25;
    while(menuya==0) menuya = (rand()%3 - 1) * 0.25;
}

void menuUpdateMapBG() {
    if(menuHasMapLoaded) {
        mxscr += menuxa;
        myscr += menuya;
        
        if (mxscr < 16) {
            mxscr = 16;
            menuxa = -menuxa;
        } else if (mxscr > 1832) {
            mxscr = 1832;
            menuxa = -menuxa;
        }
        if (myscr < 16) {
            myscr = 16;
            menuya = -menuya;
        } else if (myscr > 1792) {
            myscr = 1792;
            menuya = -menuya;
        }
    }
}

void menuRenderMapBGTop() {
    if(menuHasMapLoaded) {
        offsetX = (int) mxscr; offsetY = (int) myscr;
            renderBackground(1, (int) mxscr, (int) myscr);
        offsetX = 0; offsetY = 0;
        
        sf2d_draw_rectangle(0, 0, 400, 240, 0xAA0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    }
}

void menuRenderMapBGBottom() {
    if(menuHasMapLoaded) {
        offsetX = (int) mxscr + 20; offsetY = (int) myscr + 120;
            renderBackground(1, (int) mxscr + 20, (int) myscr + 120);
        offsetX = 0; offsetY = 0;
        
        sf2d_draw_rectangle(0, 0, 320, 240, 0xAA0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    }
}
