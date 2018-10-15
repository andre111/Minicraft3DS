#include "MenuLoadGame.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

#include <3ds.h>
#include <stdio.h>
#include <dirent.h>

#include "../editor/Editor.h"
#include "../SaveLoad.h"

char fileNames[1000][256];
int fileScore[1000];
bool fileWin[1000];
s16 worldFileCount = 0;
bool enteringName = false;
s8 errorFileName = 0;

int loadGameTarget;

bool areYouSure = false;
bool areYouSureSave = false;

void readFiles() {
    memset(&fileNames, 0, sizeof(fileNames)); // reset fileNames
    worldFileCount = 0;
    DIR * d;
    struct dirent * dir;
    d = opendir(".");
    if (d){
        while ((dir = readdir(d)) != NULL) {
            if (strncmp(dir->d_name+strlen(dir->d_name)-4, ".msv", 4) == 0) { // Check if filename contains ".msv"
                strncpy(fileNames[worldFileCount], dir->d_name, strlen(dir->d_name)-4);
                //TODO: This no longer works, update for new format:
                //FILE * file = fopen(dir->d_name, "rb");
                //fread(&fileScore[worldFileCount],sizeof(int), 1, file);
                //fread(&fileWin[worldFileCount],sizeof(bool), 1, file);
                //fclose(file);
                fileScore[worldFileCount] = 0;
                fileWin[worldFileCount] = false;
                
                ++worldFileCount;
            }
        }
        closedir(d);
    }
}



void menuLoadGameInit(int target) {
    currentSelection = 0;
    enteringName = false;
    areYouSure = false;
    
    readFiles();
    if(worldFileCount>0) {
        memset(&currentFileName, 0, 255); // reset currentFileName
        sprintf(currentFileName,"%s.msv",fileNames[currentSelection]);
        
        initBGMap = 1;
    } else {
        initBGMap = 2;
    }
    
    loadGameTarget = target;
}

void menuLoadGameTick() {
    if(!enteringName && !areYouSure){ // World select
        if (localInputs.k_decline.clicked){
            if(loadGameTarget==LOAD_TO_SINGLEPLAYER) {
                currentMenu = MENU_CHOOSE_GAME;
                currentSelection = 0;
            } else if(loadGameTarget==LOAD_TO_MULTIPLAYER) {
                currentMenu = MENU_MULTIPLAYER_HOST;
            } else {
                currentMenu = MENU_TITLE;
                currentSelection = 1;
            }
        }
        if (localInputs.k_up.clicked){ --currentSelection; if(currentSelection < 0)currentSelection = worldFileCount;}
        if (localInputs.k_down.clicked){ ++currentSelection; if(currentSelection > worldFileCount)currentSelection=0;}
    
        if(localInputs.k_delete.clicked){
            if(currentSelection < worldFileCount) areYouSure = true;
        }
    
        if(localInputs.k_accept.clicked){
            if(currentSelection == worldFileCount){
                enteringName = true;
            } else {
                memset(&currentFileName, 0, 255); // reset currentFileName
                sprintf(currentFileName, "%s.msv", fileNames[currentSelection]);
                playSound(snd_test);
                
                if(loadGameTarget==LOAD_TO_SINGLEPLAYER) {
                    initGame = 1;
                    currentMenu = MENU_LOADING;
                } else if(loadGameTarget==LOAD_TO_MULTIPLAYER) {
                    initMPGame = 2;
                    currentMenu = MENU_LOADING;
                } else if(loadGameTarget==LOAD_TO_EDITOR) {
                    editorInit();
                    currentMenu = MENU_EDITOR;
                }
            }
        }
    } else if (areYouSure){
        if (localInputs.k_decline.clicked || localInputs.k_delete.clicked) areYouSure = false;
        if (localInputs.k_accept.clicked){
            sprintf(currentFileName,"%s.msv",fileNames[currentSelection]);
            remove(currentFileName);
            readFiles();
            enteringName = false;
            areYouSure = false;
            memset(&currentFileName, 0, 255); // reset currentFileName
        }
    } else { // Enter new world name.
        if(localInputs.k_decline.clicked) enteringName = false;
        if(localInputs.k_accept.clicked && errorFileName == 0){
            errorFileName = checkFileNameForErrors(fileNames[worldFileCount]);
            if(errorFileName == 0){ // If no errors are found with the filename, then start a new game!
                memset(&currentFileName, 0, 255); // reset currentFileName
                sprintf(currentFileName, "%s.msv", fileNames[worldFileCount]);
                playSound(snd_test);
                ++worldFileCount;
                
                if(loadGameTarget==LOAD_TO_SINGLEPLAYER) {
                    initGame = 1;
                    currentMenu = MENU_LOADING;
                } else if(loadGameTarget==LOAD_TO_MULTIPLAYER) {
                    initMPGame = 2;
                    currentMenu = MENU_LOADING;
                } else if(loadGameTarget==LOAD_TO_EDITOR) {
                    editorInit();
                    currentMenu = MENU_EDITOR;
                }
            }
        }
        
        menuTickKeyboard(fileNames[worldFileCount], 24);
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            errorFileName = 0;
        }
    }
}

void menuLoadGameRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        if(!enteringName){ // World select
            offsetX = 0;offsetY = (currentSelection * 32) - 48;
            renderText("Select a file",122,-16);
            for(int i = 0; i < worldFileCount + 1; ++i){
                int color = 0xFF921020;
                char * text = fileNames[i];
                if(i == worldFileCount){
                    text = "Generate New World";
                    color = 0xFF209210;
                }
                if(i != currentSelection) color &= 0xFF7F7F7F; // Darken color.
                else {
                    if(areYouSure)color = 0xFF1010DF;
                }
                
                char scoreText[24];
                sprintf(scoreText,"Score: %d",fileScore[i]);
                
                renderFrame(1,i*4,24,(i*4)+4,color);
                if(i != worldFileCount){
                    renderText(text,(400-(strlen(text)*12))/2,i*64+12);
                    renderText(scoreText,(400-(strlen(scoreText)*12))/2,i*64+32);
                } else {
                    renderText(text,(400-(strlen(text)*12))/2,i*64+24);
                }
                if(fileWin[i] && i != worldFileCount) render16(18,i*32+8,24,208,0); // Render crown
            }
            offsetX = 0;offsetY = 0;
        } else { // Enter new world name.
            renderText("Enter a name", 128, 16);
            renderText(fileNames[worldFileCount], (400-(strlen(fileNames[worldFileCount])*12))/2, 48);
            
            if(errorFileName > 0) {
                switch(errorFileName) {// Error: Filename cannot already exist.
                    case 1: renderTextColor("ERROR: Length cannot be 0!", (400-26*12)/2, 200, 0xFF1010AF); break;    
                    case 2: renderTextColor("ERROR: You need Letters/Numbers!", (400-32*12)/2, 200, 0xFF1010AF); break;    
                    case 3: renderTextColor("ERROR: Filename already exists!", (400-31*12)/2, 200, 0xFF1010AF); break;    
                }    
            }
        }
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        if(!enteringName){ // World select
            if(!areYouSure){
                renderTextColor("Load World",100,12,0xFF3FFFFF);
                renderText("Press   or   to scroll", 28, 50);
                renderButtonIcon(localInputs.k_up.input & -localInputs.k_up.input, 98, 48, 1);
                renderButtonIcon(localInputs.k_down.input & -localInputs.k_down.input, 160, 48, 1);
                renderText("Press   to load world", (320-21*12)/2, 100);
                renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 104, 98, 1);
                renderText("Press   to return", 58, 150);
                renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
                if(currentSelection != worldFileCount){
                    renderText("Press   to delete",(320-17*12)/2, 200);
                    renderButtonIcon(localInputs.k_delete.input & -localInputs.k_delete.input, 128, 198, 1);
                }
            } else {
                renderTextColor("Delete File?",88,12,0xFF3F3FFF);
                renderText("Press   to confirm", (320-18*12)/2, 100);
                renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 122, 98, 1);
                renderText("Press   to return", 58, 150);
                renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 148, 1);
            }
        } else { // Draw the "keyboard"
            menuRenderKeyboard();
            
            renderText("Press   to confirm", (320-18*12)/2, 180);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 122, 178, 1);
            renderText("Press   to return", 58, 210);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 208, 1);
        }
    sf2d_end_frame();
}
