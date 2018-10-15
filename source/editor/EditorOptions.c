#include "EditorOptions.h"

#include "../Menu.h"
#include "../SaveLoad.h"
#include "../Render.h"

#include <string.h>

char editorOptions[][24] = {"Save", "Rename", "Return to title"};

int editorOptionsSelection;

bool editorOptionsChangeName;
char editorOrigFileName[256];
char editorCurrentFileName[256];
s8 editorErrorFileName = 0;

bool editorAreYouSure;
bool editorAreYouSureSave;
int editorMenuTimer;

void editorOptionsInit() {
    editorOptionsSelection = 0;
    editorOptionsChangeName = false;
    
    strcpy(editorOrigFileName, currentFileName);
    
    editorAreYouSure = false;
    editorAreYouSureSave = false;
    editorMenuTimer = 0;
}

void editorOptionsTick() {
    //changing filename
    if(editorOptionsChangeName) {
        if(localInputs.k_decline.clicked) editorOptionsChangeName = false;
        if(localInputs.k_accept.clicked && editorErrorFileName == 0){
            editorErrorFileName = checkFileNameForErrors(editorCurrentFileName);
            if(editorErrorFileName == 0){ // If no errors are found with the filename, then start a new game!
                memset(&currentFileName, 0, 255); // reset currentFileName
                sprintf(currentFileName, "%s.msv", editorCurrentFileName);
                editorOptionsChangeName = false;
            }
        }
        
        menuTickKeyboard(editorCurrentFileName, 24);
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            editorErrorFileName = 0;
        }
    //normal menu
    } else {
        if(editorAreYouSureSave) {
            if (localInputs.k_accept.clicked){
                //changed the filename -> copy old save so playerdata will be kept
                if(strcmp(editorOrigFileName, currentFileName)!=0) {
                    if(!saveFileCopy(currentFileName, editorOrigFileName)) {
                        return;
                    }
                }
                saveWorld(currentFileName, &eManager, &worldData, players, playerCount);
                
                editorMenuTimer = 60;
                
                editorAreYouSureSave = false;
            } else if (localInputs.k_decline.clicked){
                editorAreYouSureSave = false;
            }
        } else if(editorAreYouSure) {
            if (localInputs.k_accept.clicked){
                currentMenu = MENU_TITLE;
                currentSelection = 1;
            } else if (localInputs.k_decline.clicked){
                editorAreYouSure = false;
            }
        } else {
            if (localInputs.k_up.clicked) { --editorOptionsSelection; if(editorOptionsSelection < 0) editorOptionsSelection=2; }
            if (localInputs.k_down.clicked) { ++editorOptionsSelection; if(editorOptionsSelection > 2) editorOptionsSelection=0; }
            
            if(localInputs.k_accept.clicked) {
                switch(editorOptionsSelection) {
                    case 0:
                        editorAreYouSureSave = true;
                    break;
                    case 1:
                        editorOptionsChangeName = true;
                        //copy filename without .msv
                        strncpy(editorCurrentFileName, currentFileName, strlen(currentFileName)-4);
                        editorErrorFileName = 0;
                    break;
                    case 2:
                        editorAreYouSure = true;
                    break;
                }
            }
        }
    }
    
    if(editorMenuTimer>0) editorMenuTimer--;
}

void editorOptionsRenderTop() {
    //changing filename
    if(editorOptionsChangeName) {
        renderText("Enter a name", 128, 104);
        renderText(editorCurrentFileName, (400-(strlen(editorCurrentFileName)*12))/2, 136);
        
        if(editorErrorFileName > 0) {
            switch(editorErrorFileName) {// Error: Filename cannot already exist.
                case 1: renderTextColor("ERROR: Length cannot be 0!", (400-26*12)/2, 200, 0xFF1010AF); break;    
                case 2: renderTextColor("ERROR: You need Letters/Numbers!", (400-32*12)/2, 200, 0xFF1010AF); break;    
                case 3: renderTextColor("ERROR: Filename already exists!", (400-31*12)/2, 200, 0xFF1010AF); break;    
            }    
        }
    //normal menu
    } else {
        for(int i = 2; i >= 0; --i) {
            char* msg = editorOptions[i];
            u32 color = 0xFF7F7F7F;
            if(i == editorOptionsSelection) color = 0xFFFFFFFF;
            renderTextColor(msg, (400 - (strlen(msg) * 12))/2, (i * 24) + 112, color);    
        }
        
        if(editorMenuTimer > 0) renderTextColor("Game Saved!", (400-(11*12))/2, 200, 0xFF20FF20);
        
        if(editorAreYouSure || editorAreYouSureSave) {
            if(editorAreYouSure) renderFrame(6,5,19,10,0xFF10108F);
            else renderFrame(6,5,19,10,0xFF108F10);
            
            renderText("Are you sure?",122,96);
            renderText("   Yes", 164, 117);
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 166, 114, 1);
            renderText("   No", 170, 133);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 166, 130, 1);
        }
    }
}

void editorOptionsRenderBottom() {
    //changing filename
    if(editorOptionsChangeName) { // Draw the "keyboard"
        menuRenderKeyboard();
            
        renderText("Press   to confirm", (320-18*12)/2, 180);
        renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 122, 178, 1);
        renderText("Press   to return", 58, 210);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 208, 1);
    //normal menu
    } else {
        
    }
}
