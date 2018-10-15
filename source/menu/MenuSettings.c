#include "MenuLoading.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include "MenuSettingsRebind.h"
#include "MenuSettingsTexture.h"

char setOptions[][24] = {"Rebind Buttons", "Texture packs", "Debug Text:   ", "N3DS Speedup:   ", "Return to title"};

void menuSettingsTick() {
    if (localInputs.k_up.clicked){ 
        --currentSelection; 
        if(currentSelection == 3 && !((MODEL_3DS & 6) != 0)) --currentSelection; 
        if(currentSelection < 0) currentSelection=4;
    }
    if (localInputs.k_down.clicked){ 
        ++currentSelection; 
        if(currentSelection == 3 && !((MODEL_3DS & 6) != 0)) ++currentSelection; 
        if(currentSelection > 4) currentSelection=0;
    }
    if(localInputs.k_decline.clicked){
        currentMenu = MENU_TITLE;
        currentSelection = 3;
    }
    if(localInputs.k_accept.clicked){
        switch(currentSelection){
            case 0:
                menuSettingsRebindInit();
                currentSelection = 0;
                currentMenu = MENU_SETTINGS_REBIND;
            break;
            case 1:
                menuSettingsTextureInit();
                currentMenu = MENU_SETTINGS_TP;
                currentSelection = 0;
            break;
            case 2:
                shouldRenderDebug = !shouldRenderDebug; // toggle option
            break;
            case 3:
                if((MODEL_3DS & 6) != 0){ // detect if user is using a New 3DS
                    shouldSpeedup = !shouldSpeedup; // toggle option
                    osSetSpeedupEnable(shouldSpeedup);
                }
            break;
            case 4:
                if(true){
                    FILE * fset = fopen("settings.bin","wb");
                    fwrite(&shouldRenderDebug,sizeof(bool),1,fset);
                    fwrite(&shouldSpeedup,sizeof(bool),1,fset);
                    fclose(fset);
                }
                currentMenu = MENU_TITLE;
                currentSelection = 3;
            break;
        }
    }
}

void menuSettingsRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        renderText("Settings",(400-(8*12))/2,30);
        for(int i = 4; i >= 0; --i) {
            char* msg = setOptions[i];
            u32 color = 0xFF7F7F7F;
            if(i == currentSelection) color = 0xFFFFFFFF;
            if(i == 2) {
                if(shouldRenderDebug) renderTextColorSized("On",142, ((8 + i) * 32 - 190) >> 1,2.0, 0xFF00DF00);    
                else  renderTextColorSized("Off",142, ((8 + i) * 32 - 190) >> 1,2.0, 0xFF0000DF);   
            } else if(i == 3) {
                
                if((MODEL_3DS & 6) != 0){ // detect if user is using a New 3DS
                    if(shouldSpeedup) renderTextColorSized("On",142, ((8 + i) * 32 - 190) >> 1,2.0, 0xFF00DF00);    
                    else renderTextColorSized("Off",142, ((8 + i) * 32 - 190) >> 1,2.0, 0xFF0000DF); 
                } else {
                    color = 0xFF3F3F3F;
                    renderTextColorSized("Off",142, ((8 + i) * 32 - 190) >> 1,2.0, 0xFF3F3F3F); 
                }
            }
            renderTextColorSized(msg,(200 - (strlen(msg) * 8))/2, ((8 + i) * 32 - 190) >> 1,2.0, color);    
        }
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);    
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        switch(currentSelection) {
            case 0:
                renderTextColor("Change the controls",(320 - (19 * 12))/2,24,0xFF7FFFFF);
            break;
            case 1:
                renderTextColor("Change the game's art",(320 - (21 * 12))/2,24,0xFF7FFFFF);
            break;
            case 2:
                renderTextColor("Show FPS/Pos/Entities",(320 - (22 * 12))/2,24,0xFF7FFFFF);
            break;
            case 3:
                renderTextColor("Use the N3DS 804mhz mode",(320 - (24 * 12))/2,24,0xFF7FFFFF);
            break;
            case 4:
                renderTextColor("Back to the titlescreen",(320 - (23 * 12))/2,24,0xFF7FFFFF);
            break;
        }
        renderText("Press   to select", 58, 100);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 128, 98, 1);
        renderText("Press   to return", 58, 150);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
    sf2d_end_frame();
}
