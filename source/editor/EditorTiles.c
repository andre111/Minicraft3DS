#include "EditorTiles.h"

#include "../Globals.h"
#include "../Ingame.h"
#include "../Render.h"

#define EDITOR_MODE_PAINT 0
#define EDITOR_MODE_PICK 1
#define EDITOR_MODE_SCROLL 2
#define EDITOR_MODE_CHOOSE 3

int editorX;
int editorY;
int editorLevel;

int editorMode;

bool editorTouchDragging;
s16 editorTouchXLast;
s16 editorTouchYLast;

int editorTile;

int editorChooseOffset;

void editorTilesInit() {
    editorX = 0;
    editorY = 0;
    editorLevel = 1;
    
    editorMode = EDITOR_MODE_SCROLL;
    
    editorTouchDragging = false;
    editorTouchXLast = 0;
    editorTouchYLast = 0;
    
    editorTile = 0;
    
    editorChooseOffset = 0;
}

void editorTilesTick() {
    //update mode (only when not currently drawing/dragging/...)
    if(localInputs.k_touch.px <= 0 && localInputs.k_touch.py <= 0) {
        editorMode = EDITOR_MODE_SCROLL;
        if(localInputs.k_up.down) {
            editorMode = EDITOR_MODE_PAINT;
        } else if(localInputs.k_left.down) {
            editorMode = EDITOR_MODE_PICK;
        } else if(localInputs.k_right.down) {
            editorMode = EDITOR_MODE_CHOOSE;
        }
        
        if(localInputs.k_accept.clicked) {
            if(editorLevel>0) editorLevel--;
        }
        if(localInputs.k_decline.clicked) {
            if(editorLevel<4) editorLevel++;
        }
    }
    
    //painting tiles
    if(editorMode==EDITOR_MODE_PAINT) {
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            int tilePosX = (editorX + localInputs.k_touch.px/2) >> 4;
            int tilePosY = (editorY + localInputs.k_touch.py/2) >> 4;
            
            setTile(editorTile, editorLevel, tilePosX, tilePosY);
        }
    //picking tiles
    } else if(editorMode==EDITOR_MODE_PICK) {
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            int tilePosX = (editorX + localInputs.k_touch.px/2) >> 4;
            int tilePosY = (editorY + localInputs.k_touch.py/2) >> 4;
            
            editorTile = getTile(editorLevel, tilePosX, tilePosY);
        }
    //dragging the map
    } else if(editorMode==EDITOR_MODE_SCROLL) {
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            if(editorTouchDragging) {
                s16 moveX = (editorTouchXLast - localInputs.k_touch.px)/2;
                s16 moveY = (editorTouchYLast - localInputs.k_touch.py)/2;
                
                editorX += moveX;
                editorY += moveY;
                
                if(editorX<0) editorX = 0;
                if(editorX>128*16-320) editorX = 128*16-320;
                if(editorY<0) editorY = 0;
                if(editorY>128*16-240) editorY = 128*16-240;
            } else {
                editorTouchDragging = true;
            }
            editorTouchXLast = localInputs.k_touch.px;
            editorTouchYLast = localInputs.k_touch.py;
        } else {
            editorTouchDragging = false;
        }
    //choosing tiles from all possible
    } else if(editorMode==EDITOR_MODE_CHOOSE) {
        if(localInputs.k_touch.px > 0 || localInputs.k_touch.py > 0) {
            if(editorTouchDragging) {
                s16 moveY = (editorTouchYLast - localInputs.k_touch.py)/2;
                
                editorChooseOffset += moveY;
                
                if(editorChooseOffset<0) editorChooseOffset = 0;
                if(editorChooseOffset>(MAX_TILE_ID_USED+5)/6 * 24 - (240/2 - 24)) editorChooseOffset = (MAX_TILE_ID_USED+5)/6 * 24 - (240/2 - 24);
            } else {
                if(localInputs.k_touch.px > (24*2)*6) {
                    editorTouchDragging = true;
                } else {
                    int touchTile = (localInputs.k_touch.px/(24*2)) + ((localInputs.k_touch.py + editorChooseOffset*2)/(24*2))*6;
                    if(touchTile >= 0 && touchTile <= MAX_TILE_ID_USED) {
                        editorTile = touchTile;
                    }
                }
            }
            editorTouchXLast = localInputs.k_touch.px;
            editorTouchYLast = localInputs.k_touch.py;
        } else {
            editorTouchDragging = false;
        }
    }
}

//abuses dungeon level for rendering
void editorRenderTile(int id, int data, int x, int y) {
    int xt, yt;
    
    for(xt = 0; xt < 3; xt++) {
        for(yt = 0; yt < 3; yt++) {
            setTile(TILE_NULL, 5, xt, yt);
        }
    }
    setTile(id, 5, 1, 1);
    setData(data, 5, 1, 1);
    offsetX = -x;
    offsetY = -y;
    renderTile(id, data, 5, 1, 1);
    offsetX = 0;
    offsetY = 0;
}

void editorTilesRenderTop() {
    //render current tile
    renderFrame(0, 4, 10, 15, 0xFFFF1010);
    
    renderTextColor("Current Tile", 80-(12*12)/2+1, 80+1, 0xFF000000);
    renderTextColor("Current Tile", 80-(12*12)/2, 80, 0xFFFFFFFF);
    
    editorRenderTile(editorTile, 0, 32, 50);
    
    renderTextColor(tileGetName(editorTile), 80-(strlen(tileGetName(editorTile))*12)/2+1, 140+1, 0xFF000000);
    renderTextColor(tileGetName(editorTile), 80-(strlen(tileGetName(editorTile))*12)/2, 140, 0xFFFFFFFF);
    
    //render current level number
    char sbuffer[10];
    sprintf(sbuffer, "Level: %i", editorLevel);
    renderTextColor(sbuffer, 8+1, 180+1, 0xFF000000);
    renderTextColor(sbuffer, 8, 180, 0xFFFFFFFF);
    
    renderTextColor("   up", 8+1, 200+1, 0xFF000000);
    renderTextColor("   up", 8, 200, 0xFFFFFFFF);
    renderTextColor("   down", 8+1, 218+1, 0xFF000000);
    renderTextColor("   down", 8, 218, 0xFFFFFFFF);
    
    renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 20, 200-3, 1);
    renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 20, 218-3, 1);
    
    
    //draw dp with available modes
    renderFrame(10, 4, 25, 15, 0xFFFF1010);
    int dpxpos = 280;
    int dpypos = 152;
    
    sf2d_draw_rectangle(dpxpos-32/2, dpypos-32*3/2, 32, 32*3, 0xFF606060);
    sf2d_draw_rectangle(dpxpos-32*3/2, dpypos-32/2, 32*3, 32, 0xFF606060);
    
    if(editorMode==EDITOR_MODE_SCROLL) {}
    else if(editorMode==EDITOR_MODE_PICK) sf2d_draw_rectangle(dpxpos-32*3/2, dpypos-32/2, 32, 32, 0xFF909090);
    else if(editorMode==EDITOR_MODE_PAINT) sf2d_draw_rectangle(dpxpos-32/2, dpypos-32*3/2, 32, 32, 0xFF909090);
    else if(editorMode==EDITOR_MODE_CHOOSE) sf2d_draw_rectangle(dpxpos+32/2, dpypos-32/2, 32, 32, 0xFF909090);
    else sf2d_draw_rectangle(dpxpos-32/2, dpypos+32/2, 32, 32, 0xFF909090);
    
    //draw texts for modes
    renderTextColor("Scroll", dpxpos-(6*12)/2+1, dpypos-6+1, 0xFF000000);
    renderTextColor("Scroll", dpxpos-(6*12)/2, dpypos-6, 0xFF404040);
    
    renderTextColor("Pick", dpxpos-56-(4*12)+1, dpypos-6+1, 0xFF000000);
    renderTextColor("Pick", dpxpos-56-(4*12), dpypos-6, 0xFF404040);
    
    renderTextColor("Paint", dpxpos-(5*12)/2+1, dpypos-64+1, 0xFF000000);
    renderTextColor("Paint", dpxpos-(5*12)/2, dpypos-64, 0xFF404040);
    
    renderTextColor("Tile", dpxpos+56+1, dpypos-6+1, 0xFF000000);
    renderTextColor("Tile", dpxpos+56, dpypos-6, 0xFF404040);
    
    renderTextColor("Tests", dpxpos-(5*12)/2+1, dpypos+54+1, 0xFF000000);
    renderTextColor("Tests", dpxpos-(5*12)/2, dpypos+54, 0xFF404040);
    
    if(editorMode==EDITOR_MODE_SCROLL) renderTextColor("Scroll", dpxpos-(6*12)/2, dpypos-6, 0xFFFFFFFF);
    else if(editorMode==EDITOR_MODE_PICK) renderTextColor("Pick", dpxpos-56-(4*12), dpypos-6, 0xFFFFFFFF);
    else if(editorMode==EDITOR_MODE_PAINT) renderTextColor("Paint", dpxpos-(5*12)/2, dpypos-64, 0xFFFFFFFF);
    else if(editorMode==EDITOR_MODE_CHOOSE) renderTextColor("Tile", dpxpos+56, dpypos-6, 0xFFFFFFFF);
    else {}
}

void editorTilesRenderBottom() {
    if(editorMode==EDITOR_MODE_PAINT || editorMode==EDITOR_MODE_PICK || editorMode==EDITOR_MODE_SCROLL) {
        //render map
        offsetX = editorX;
        offsetY = editorY;
        renderBackground(editorLevel, editorX, editorY);
        offsetX = 0;
        offsetY = 0;
    } else if(editorMode==EDITOR_MODE_CHOOSE) {
        int t;
        
        for (t = 0; t <= MAX_TILE_ID_USED; t++) {
            int xp = (t%6);
            int yp = (t/6);
            
            offsetY = editorChooseOffset;
            renderFrame(xp*3, yp*3, xp*3+3, yp*3+3, 0xFFFF1010);
            offsetY = 0;
            
            editorRenderTile(t, 0, xp*24+3, yp*24+3 - editorChooseOffset);
        }
    }
}
