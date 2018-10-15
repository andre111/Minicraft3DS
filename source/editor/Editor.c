#include "Editor.h"

#include "EditorTiles.h"
#include "EditorOptions.h"

#include "../Globals.h"
#include "../Ingame.h"
#include "../Render.h"

#define EDITOR_COMPONENT_TILES 0
#define EDITOR_COMPONENT_ENTITIES 1
#define EDITOR_COMPONENT_SCRIPT 2
#define EDITOR_COMPONENT_OPTIONS 3

int editorComponent;

bool editorTouchDragging;
s16 editorTouchXLast;
s16 editorTouchYLast;

int editorTile;

int editorChooseOffset;

void editorInit() {
    //setup editor specific values
    playerCount = 0;
    bool load = false;
    char * filename = currentFileName;
    
    FILE *file = fopen(filename, "rb");
    if(file!=NULL) {
        fclose(file);
        load = true;
    }
    
    //call game start
    //TODO: This currently works, because startGame with playerCount=0 does nothing but load the game, when that changes, this will break
    startGame(load, filename);
    
    //initialise editor components
    editorComponent = 0;
    
    editorTilesInit();
    editorOptionsInit();
}

void editorTick() {
    if(localInputs.k_menuPrev.clicked) {
        if(editorComponent>0) editorComponent--;
    }
    if(localInputs.k_menuNext.clicked) {
        if(editorComponent<3) editorComponent++;
    }
    
    switch(editorComponent) {
    case EDITOR_COMPONENT_TILES:
        editorTilesTick();
    break;
    case EDITOR_COMPONENT_ENTITIES:
    
    break;
    case EDITOR_COMPONENT_SCRIPT:
    
    break;
    case EDITOR_COMPONENT_OPTIONS:
        editorOptionsTick();
    break;
    }
}

char* editorGetComponentName(int comp) {
    switch(comp) {
    case EDITOR_COMPONENT_TILES:
        return "Tiles";
    case EDITOR_COMPONENT_ENTITIES:
        return "Entities";
    case EDITOR_COMPONENT_SCRIPT:
        return "Script";
    case EDITOR_COMPONENT_OPTIONS:
        return "Options";
    default:
        return "";
    }
}

void editorRender() {
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        //draw top bar
        renderFrame(0, 0, 25, 4, 0xFF209210);
        renderTextColor("Editor Mode:", 200-(12*12)/2+1, 12+1, 0xFF000000);
        renderTextColor("Editor Mode:", 200-(12*12)/2, 12, 0xFFFFFFFF);
        
        char* text;
        text = editorGetComponentName(editorComponent-1);
        renderTextColor(text, 90-(strlen(text)*12)/2+1, 40+1, 0xFF000000);
        renderTextColor(text, 90-(strlen(text)*12)/2, 40, 0xFF404040);
        
        text = editorGetComponentName(editorComponent);
        renderTextColor(text, 200-(strlen(text)*12)/2+1, 40+1, 0xFF000000);
        renderTextColor(text, 200-(strlen(text)*12)/2, 40, 0xFFFFFFFF);
        
        text = editorGetComponentName(editorComponent+1);
        renderTextColor(text, 310-(strlen(text)*12)/2+1, 40+1, 0xFF000000);
        renderTextColor(text, 310-(strlen(text)*12)/2, 40, 0xFF404040);
        
        
        renderButtonIcon(localInputs.k_menuPrev.input & -localInputs.k_menuPrev.input, 16, 40-3, 1);
        renderButtonIcon(localInputs.k_menuNext.input & -localInputs.k_menuNext.input, 364, 40-3, 1);
        
        //render component
        switch(editorComponent) {
        case EDITOR_COMPONENT_TILES:
            editorTilesRenderTop();
        break;
        case EDITOR_COMPONENT_ENTITIES:
        
        break;
        case EDITOR_COMPONENT_SCRIPT:
        
        break;
        case EDITOR_COMPONENT_OPTIONS:
            editorOptionsRenderTop();
        break;
        }
    sf2d_end_frame();
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        //render component
        switch(editorComponent) {
        case EDITOR_COMPONENT_TILES:
            editorTilesRenderBottom();
        break;
        case EDITOR_COMPONENT_ENTITIES:
        
        break;
        case EDITOR_COMPONENT_SCRIPT:
        
        break;
        case EDITOR_COMPONENT_OPTIONS:
            editorOptionsRenderBottom();
        break;
        }
    sf2d_end_frame();
}
