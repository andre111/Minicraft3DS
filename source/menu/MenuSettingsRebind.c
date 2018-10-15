#include "MenuSettingsRebind.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

char keybOptions[][24] = {"Exit and Save", "Exit and Don't save", "Reset to default"};

int keys[] = {
    KEY_A,KEY_B,KEY_X,KEY_Y,
    KEY_CPAD_UP,KEY_CPAD_DOWN,KEY_CPAD_LEFT,KEY_CPAD_RIGHT,
    KEY_DUP, KEY_DDOWN, KEY_DLEFT, KEY_DRIGHT, 
    KEY_CSTICK_UP,KEY_CSTICK_DOWN,KEY_CSTICK_LEFT,KEY_CSTICK_RIGHT,
    KEY_L,KEY_R,KEY_ZL,KEY_ZR,
    KEY_START,KEY_SELECT
};

int keyProp[12] = {[0 ... 11] = 0};
bool left = false;
bool selBut = false;
bool bindOpt = false;
s8 errorBut = -1;
s8 curSaveSel = 0;

void switchGameBut(bool left, int buttonID){
    int id;
    for(id = 0; id < 7; ++id){
        if(keyProp[id] & buttonID){
            keyProp[id] ^= buttonID; // Toggle buttonID bit
            if(left){
                int id2 = id -1;
                if (id2 < 0) return;
                keyProp[id2] ^= buttonID;
            } else { 
                int id2 = id+1;
                if (id2 > 6) return;
                keyProp[id2] ^= buttonID;
            }
            return;
        }
    }
    if(left) keyProp[6] ^= buttonID;
    else keyProp[0] ^= buttonID;
    
}

void switchMenuBut(bool left, int buttonID){
    int id;
    for(id = 0; id < 12; ++id){
        if(id > 3 && id < 7) continue;
        if(keyProp[id] & buttonID){
            keyProp[id] ^= buttonID; // Toggle buttonID bit
            if(left){
                int id2 = id - 1;
                if (id2 == 6) id2 = 3;
                if (id2 < 0) return;
                keyProp[id2] ^= buttonID;
            } else { 
                int id2 = id+1;
                if (id2 == 4) id2 = 7;
                if (id2 > 11) return;
                keyProp[id2] ^= buttonID;
            }
            return;
        }
    }
    if(left) keyProp[11] ^= buttonID;
    else keyProp[0] ^= buttonID;
}

s8 checkPropButtons(){
    if(keyProp[0] == 0) return 0;
    if(keyProp[1] == 0) return 1;
    if(keyProp[2] == 0) return 2;
    if(keyProp[3] == 0) return 3;
    if(keyProp[4] == 0) return 4;
    if(keyProp[5] == 0) return 5;
    if(keyProp[6] == 0) return 6;
    if(keyProp[7] == 0) return 7;
    if(keyProp[8] == 0) return 8;
    if(keyProp[9] == 0) return 9;
    if(keyProp[10] == 0) return 10;
    if(keyProp[11] == 0) return 11;
    return -1;
}

char * getButtonFunctionGame(int key){
    if(keyProp[0] & key) return "Move up";
    if(keyProp[1] & key) return "Move down";
    if(keyProp[2] & key) return "Move left";
    if(keyProp[3] & key) return "Move right";
    if(keyProp[4] & key) return "Attack";
    if(keyProp[5] & key) return "Toggle Menu";
    if(keyProp[6] & key) return "Pause";
    return "Nothing";
}

char * getButtonFunctionMenu(int key){
    if(keyProp[0] & key) return "Up";
    if(keyProp[1] & key) return "Down";
    if(keyProp[2] & key) return "Left";
    if(keyProp[3] & key) return "Right";
    if(keyProp[7] & key) return "Accept";
    if(keyProp[8] & key) return "Decline";
    if(keyProp[9] & key) return "Delete";
    if(keyProp[10] & key) return "Next";
    if(keyProp[11] & key) return "Previous";
    return "Nothing";
}



void menuSettingsRebindInit() {
    keyProp[0] = localInputs.k_up.input;
    keyProp[1] = localInputs.k_down.input;
    keyProp[2] = localInputs.k_left.input;
    keyProp[3] = localInputs.k_right.input;
    keyProp[4] = localInputs.k_attack.input;
    keyProp[5] = localInputs.k_menu.input;
    keyProp[6] = localInputs.k_pause.input;
    keyProp[7] = localInputs.k_accept.input;
    keyProp[8] = localInputs.k_decline.input;
    keyProp[9] = localInputs.k_delete.input;
    keyProp[10] = localInputs.k_menuNext.input;
    keyProp[11] = localInputs.k_menuPrev.input;
    left = true;
    selBut = false;
    bindOpt = false;
}

void menuSettingsRebindTick() {
    if(!bindOpt){
        if(!selBut){
            if (localInputs.k_up.clicked) { --currentSelection; if(currentSelection < 0)currentSelection=21; }
            if (localInputs.k_down.clicked) { ++currentSelection; if(currentSelection > 21)currentSelection=0; }
            if (localInputs.k_left.clicked) { left = true; }
            if (localInputs.k_right.clicked) { left = false; }
        } else {
            if (localInputs.k_left.clicked) { 
                if(left)switchGameBut(true,keys[currentSelection]);
                else switchMenuBut(true,keys[currentSelection]);
            } else if (localInputs.k_right.clicked) { 
                if(left)switchGameBut(false,keys[currentSelection]);
                else switchMenuBut(false,keys[currentSelection]);
            }
        }
        
        if (localInputs.k_accept.clicked) selBut = !selBut;
        if (localInputs.k_decline.clicked) { 
            bindOpt = true;
            curSaveSel = 0;
        }
    } else {
        if (localInputs.k_up.clicked) { --curSaveSel; if(curSaveSel < 0)curSaveSel=2; }
        if (localInputs.k_down.clicked) { ++curSaveSel; if(curSaveSel > 2)curSaveSel=0; }
        if (localInputs.k_decline.clicked) { 
            bindOpt = false;
            errorBut = -1;
        }
        if (localInputs.k_accept.clicked) {
            switch(curSaveSel) {
                case 0: // Exit and save  
                    if(checkPropButtons() == -1) {
                        localInputs.k_up.input = keyProp[0];
                        localInputs.k_down.input = keyProp[1];
                        localInputs.k_left.input = keyProp[2];
                        localInputs.k_right.input = keyProp[3];
                        localInputs.k_attack.input = keyProp[4];
                        localInputs.k_menu.input = keyProp[5];
                        localInputs.k_pause.input = keyProp[6];
                        localInputs.k_accept.input = keyProp[7];
                        localInputs.k_decline.input = keyProp[8];
                        localInputs.k_delete.input = keyProp[9];
                        localInputs.k_menuNext.input = keyProp[10];
                        localInputs.k_menuPrev.input = keyProp[11];
                        
                        FILE *fs=fopen("btnSave.bin","wb");
                        fwrite(keyProp,sizeof(int),12,fs);
                        fclose(fs);
                        
                        currentSelection = 0;
                        currentMenu = MENU_SETTINGS;
                        errorBut = -1;
                    } else {
                        errorBut = checkPropButtons();
                    }
                break;
                case 1: // Exit and DON'T save  
                    currentSelection = 0;
                    currentMenu = MENU_SETTINGS;
                    errorBut = -1;
                break;
                case 2: // reset defaults
                    keyProp[0] = KEY_DUP | KEY_CPAD_UP | KEY_CSTICK_UP;
                    keyProp[1] = KEY_DDOWN | KEY_CPAD_DOWN | KEY_CSTICK_DOWN;
                    keyProp[2] = KEY_DLEFT | KEY_CPAD_LEFT | KEY_CSTICK_LEFT;
                    keyProp[3] = KEY_DRIGHT | KEY_CPAD_RIGHT | KEY_CSTICK_RIGHT;
                    keyProp[4] = KEY_A | KEY_B | KEY_L | KEY_ZR;
                    keyProp[5] = KEY_X | KEY_Y | KEY_R | KEY_ZL;
                    keyProp[6] = KEY_START;
                    keyProp[7] = KEY_A;
                    keyProp[8] = KEY_B;
                    keyProp[9] = KEY_X;
                    keyProp[10] = KEY_R;
                    keyProp[11] = KEY_L;
                    bindOpt = false;
                    errorBut = -1;
                break;
            }
        }
    }
}

void menuSettingsRebindRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        renderTextColor("Rebind Buttons",116,12,0xFF00AFAF);
        renderText("Button",16,32);
        renderText("Game",140,32);
        renderText("Menus",280,32);
    
        char gameButText[34];
        char menuButText[34];
            
        for(int i = 0; i < 5; ++i){
            if((currentSelection-2) + i > 21 || (currentSelection-2) + i < 0) continue;
            renderButtonIcon(keys[(currentSelection-2) + i], 16, (i * 18) + 30, 2);
            int ccol = 0xFF7F7F7F;
            
            sprintf(gameButText,"%s",getButtonFunctionGame(keys[(currentSelection-2) + i]));
            sprintf(menuButText,"%s",getButtonFunctionMenu(keys[(currentSelection-2) + i]));
            
            if(i == 2){ 
                if(!selBut)ccol = 0xFFFFFFFF;
                else{ 
                    ccol = 0xFF00FF00;
                    if(left)sprintf(gameButText,"<%s>",getButtonFunctionGame(keys[(currentSelection-2) + i]));
                    else sprintf(menuButText,"<%s>",getButtonFunctionMenu(keys[(currentSelection-2) + i]));
                }
            }
            if(left){
                renderTextColor(gameButText, 112, (i * 33) + 80, ccol);
                renderTextColor(menuButText, 280, (i * 33) + 80, 0xFF7F7F7F);
            } else {
                renderTextColor(gameButText, 112, (i * 33) + 80, 0xFF7F7F7F);
                renderTextColor(menuButText, 280, (i * 33) + 80, ccol);
            }
        }
        
        if(bindOpt){
            renderFrame(1,1,24,14,0xFFBF1010);
            renderTextColor("Save changes?",122,32,0xFF00AFAF);
            for(int i = 2; i >= 0; --i){
                char* msg = keybOptions[i];
                u32 color = 0xFF4F4F4F;
                if(i == curSaveSel) color = 0xFFFFFFFF;
                renderTextColor(msg,(400 - (strlen(msg) * 12))/2, (i * 24) + 92, color);    
            }
            renderText("Press   to return", 98, 190);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 168, 188, 1);
            
            if(errorBut >= 0 && errorBut < 12){
                char errorText[30];
                switch(errorBut){
                    case 0: sprintf(errorText, "Error: Missing 'Move up'"); break;
                    case 1: sprintf(errorText, "Error: Missing 'Move down'"); break;
                    case 2: sprintf(errorText, "Error: Missing 'Move right'"); break;
                    case 3: sprintf(errorText, "Error: Missing 'Move left'"); break;
                    case 4: sprintf(errorText, "Error: Missing 'Attack'"); break;
                    case 5: sprintf(errorText, "Error: Missing 'Toggle Menu'"); break;
                    case 6: sprintf(errorText, "Error: Missing 'Pause'"); break;
                    case 7: sprintf(errorText, "Error: Missing 'Accept'"); break;
                    case 8: sprintf(errorText, "Error: Missing 'Decline'"); break;
                    case 9: sprintf(errorText, "Error: Missing 'Delete'"); break;
                    case 10: sprintf(errorText, "Error: Missing 'Next'"); break;
                    case 11: sprintf(errorText, "Error: Missing 'Previous'"); break;
                }
                renderTextColor(errorText,(400 - (strlen(errorText) * 12))/2,50,0xFF0000FF);
            }
        }
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);    
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        if(!selBut){
            renderText("Press   to select", 58, 80);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 128, 78, 1);
            renderText("Press   to return", 58, 130);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 128, 1);
        } else {
            renderText("Press   or   to scroll", 28, 50);
            renderButtonIcon(localInputs.k_left.input & -localInputs.k_left.input, 98, 48, 1);
            renderButtonIcon(localInputs.k_right.input & -localInputs.k_right.input, 160, 48, 1);
            renderText("Press   to unselect", 46, 100);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 118, 98, 1);
            renderText("Press   to return", 58, 150);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
        }
    sf2d_end_frame();
}
