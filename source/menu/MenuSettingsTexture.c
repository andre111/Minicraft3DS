#include "MenuSettingsTexture.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include <3ds.h>
#include <stdio.h>
#include <dirent.h>

#include "../texturepack.h"

char tpFileNames[1000][256];
char tpFileComment[1000][60];
s16 tpFileCount = 0;
s8 isLoadingTP = 0;

void readTPFiles(){
    memset(&tpFileNames, 0, sizeof(tpFileNames)); // reset tp fileNames
    memset(&tpFileComment, 0, sizeof(tpFileComment)); // reset zip comments
    tpFileCount = 1; // 0 = default.
    DIR * d;
    struct dirent * dir;
    d = opendir("./texturepacks/");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".zip") != NULL) { // Check if filename contains ".zip"
                strncpy(tpFileNames[tpFileCount], dir->d_name, strlen(dir->d_name)-4);
                
                char fullDirName[256];
                sprintf(fullDirName,"./texturepacks/%s",dir->d_name);
                //int err = 
                getTexturePackComment(fullDirName, tpFileComment[tpFileCount]);
                /*
                if(err > 0){
                    char errorText[10];    
                    sprintf(errorText,"err:%d",err);
                    strncpy(tpFileComment[tpFileCount], errorText, strlen(errorText));
                }*/
                ++tpFileCount;
            }
        }
        closedir(d);
    }
}



void menuSettingsTextureInit() {
    readTPFiles();
}

void menuSettingsTextureTick() {
    if (localInputs.k_up.clicked) { --currentSelection; if(currentSelection < 0) currentSelection = tpFileCount-1; }
    if (localInputs.k_down.clicked) { ++currentSelection; if(currentSelection > tpFileCount-1) currentSelection=0; }
    if (localInputs.k_decline.clicked) {
        if(isLoadingTP < 1) {
            currentMenu = MENU_SETTINGS;
            currentSelection = 1;
        }
    }
    if (localInputs.k_accept.clicked) {
        if(currentSelection > 0) {
            isLoadingTP = 4;
        } else {
            if(icons!=NULL) {
                sf2d_free_texture(icons);
                icons = NULL;
            }
            icons = sfil_load_PNG_buffer(icons_png, SF2D_PLACE_RAM);
            reloadColors();
            
            if(font!=NULL) {
                sf2d_free_texture(font);
                font = NULL;
            }
            font = sfil_load_PNG_buffer(Font_png, SF2D_PLACE_RAM);
            
            if(bottombg!=NULL) {
                sf2d_free_texture(bottombg);
                bottombg = NULL;
            }
            bottombg = sfil_load_PNG_buffer(bottombg_png, SF2D_PLACE_RAM);
            
            if(playerSprites!=NULL) {
                sf2d_free_texture(playerSprites);
                playerSprites = NULL;
            }
            playerSprites = sfil_load_PNG_buffer(player_png, SF2D_PLACE_RAM);
            
            currentMenu = MENU_SETTINGS;
            currentSelection = 1;
            remove("lastTP.bin");
        }
    }
}

void menuSettingsTextureRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        offsetX = 0; offsetY = (currentSelection * 40) - 48;
    
        renderText("Texture Packs",122,-16);
        for(int i = 0; i < tpFileCount; ++i) {
            int color = 0xFF923232;
            char * text = tpFileNames[i];
            char * cmtText = tpFileComment[i];
            if(i == 0){
                text = "Default";
                cmtText = "Regular look of the game";
                color = 0xFF921060;
            }
            if(i != currentSelection) color &= 0x7FFFFFFF; // Darken color.
            
            renderFrame(1,i*5,24,(i*5)+5,color);
            renderText(text,(400-(strlen(text)*12))/2,i*80+16);
            
            if(strlen(cmtText) > 29) {
                char cmtTxt1[30],cmtTxt2[30];
                strncpy(cmtTxt1, cmtText, 29);
                strncpy(cmtTxt2, cmtText + 29, strlen(cmtText)-29);
                renderTextColor(cmtTxt1,(400-(strlen(cmtTxt1)*12))/2,i*80+36,0xFFAFAFAF);
                renderTextColor(cmtTxt2,(400-(strlen(cmtTxt2)*12))/2,i*80+50,0xFFAFAFAF);
            } else {
                renderTextColor(cmtText,(400-(strlen(cmtText)*12))/2,i*80+43,0xFFAFAFAF);
            }
        }
        
        offsetX = 0; offsetY = 0;
        if(isLoadingTP > 0) {
            --isLoadingTP;
            renderFrame(1,5,24,9,0xFF666666);
            renderTextColor("Loading Texture pack...",(400-(23*12))/2,108,0xFF10FFFF);
            if(isLoadingTP == 0){
                char fullDirName[256];
                sprintf(fullDirName,"./texturepacks/%s.zip",tpFileNames[currentSelection]);
                loadedtp = loadTexturePack(fullDirName);   
                
                FILE * fs=fopen("lastTP.bin","w");
                fprintf(fs,"%s", fullDirName);
                fclose(fs);
                
                currentMenu = MENU_SETTINGS;
                currentSelection = 1; 
            }
        }
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);    
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        renderText("Press   to select", 58, 100);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 128, 98, 1);
        renderText("Press   to return", 58, 150);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
    sf2d_end_frame();
}
