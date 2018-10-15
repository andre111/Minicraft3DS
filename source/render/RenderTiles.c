#include "RenderTiles.h"

#include "Batching.h"
#include "../Render.h"

//internal values and methods
static bool inBatch = false;

static bool tu = false;
static bool td = false;
static bool tl = false;
static bool tr = false;

static bool tul = false;
static bool tur = false;
static bool tdl = false;
static bool tdr = false;

static inline void tileRender(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, s32 wTile, s32 hTile) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += wTile;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += hTile;
    }
    
    if(inBatch) {
        batch_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, wTile, hTile, scaleX, scaleY);
    } else {
        sf2d_draw_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, wTile, hTile, scaleX, scaleY);
    }
}

static void tileRender8(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits) {
    tileRender(xp, yp, xTile, yTile, bits, 8, 8);
}

static void tileRender16(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits) {
    tileRender(xp, yp, xTile, yTile, bits, 16, 16);
}

static void renderDots(int x, int y, u8 bits1, u8 bits2, u8 bits3, u8 bits4, u32 xTile, u32 yTile) {
    //another speedhack for o3DS
    if(tu && tl && tr && td) {
        tileRender16(x, y, xTile, yTile, bits1);
        return;
    }
    
    if(tu && tl) tileRender8(x, y, xTile, yTile, bits1);
    if(tu && tr) tileRender8(x + 8, y, xTile+8, yTile, bits2);
    if(td && tl) tileRender8(x, y + 8, xTile, yTile+8, bits3);
    if(td && tr) tileRender8(x + 8, y + 8, xTile+8, yTile+8, bits4);
}

static void resetSurrTiles() {
    tu = false;
    td = false;
    tl = false;
    tr = false;

    tul = false;
    tur = false;
    tdl = false;
    tdr = false;
}

static void checkSurrTiles8(u8 level, int xt, int yt, int id) {
    if (getTile(level, xt, yt - 1) == id)
        tu = true;
    if (getTile(level, xt - 1, yt) == id)
        tl = true;
    if (getTile(level, xt + 1, yt) == id)
        tr = true;
    if (getTile(level, xt, yt + 1) == id)
        td = true;
    if (getTile(level, xt - 1, yt - 1) == id)
        tul = true;
    if (getTile(level, xt + 1, yt - 1) == id)
        tur = true;
    if (getTile(level, xt - 1, yt + 1) == id)
        tdl = true;
    if (getTile(level, xt + 1, yt + 1) == id)
        tdr = true;
}

static void checkSurrTiles4(u8 level, int xt, int yt, int id) {
    if (getTile(level, xt, yt - 1) == id)
        tu = true;
    if (getTile(level, xt - 1, yt) == id)
        tl = true;
    if (getTile(level, xt + 1, yt) == id)
        tr = true;
    if (getTile(level, xt, yt + 1) == id)
        td = true;
}


//"public" methods
u8 tData = 0;
void renderTile(int i, int d, u8 level, int x, int y) {
    int age = 0;
    switch (i) {
    case TILE_GRASS:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GRASS);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_TREE);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_FLOWER);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAPLING_TREE);
        
        if(level==1 && worldData.season==3) renderConnectedTile4(x, y, 256, 112);
        else if(level==1 && worldData.season==2) renderConnectedTile4(x, y, 256, 128);
        else renderConnectedTile4(x, y, 256, 0);
        break;
    case TILE_TREE:
        renderTile(TILE_GRASS, 0, level, x, y);
        
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_TREE);
        
        if(worldData.season==2) {
            tileRender8(x,   y,   352+((tu && tl && tul) ? 16 : 0), 96, 0);
            tileRender8(x+8, y,   360+((tu && tr && tur) ? 16 : 0), 96, 0);
            tileRender8(x,   y+8, 352+((td && tl && tdl) ? 16 : 0), 104, 0);
            tileRender8(x+8, y+8, 360+((td && tr && tdr) ? 16 : 0), 104, 0);
        } else if(worldData.season==3) {
            tileRender8(x,   y,   352+((tu && tl && tul) ? 16 : 0), 112, 0);
            tileRender8(x+8, y,   360+((tu && tr && tur) ? 16 : 0), 112, 0);
            tileRender8(x,   y+8, 352+((td && tl && tdl) ? 16 : 0), 120, 0);
            tileRender8(x+8, y+8, 360+((td && tr && tdr) ? 16 : 0), 120, 0);
        } else {
            tileRender8(x,   y,   256+((tu && tl && tul) ? 16 : 0), 48, 0);
            tileRender8(x+8, y,   264+((tu && tr && tur) ? 16 : 0), 48, 0);
            tileRender8(x,   y+8, 256+((td && tl && tdl) ? 16 : 0), 56, 0);
            tileRender8(x+8, y+8, 264+((td && tr && tdr) ? 16 : 0), 56, 0);
        }
        
        break;
    case TILE_ROCK:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_ROCK);
        if(level>1)
            renderConnectedTile8(x, y, 256, 96);
        else
            renderConnectedTile8(x, y, 336, 64);
        break;
    case TILE_HARDROCK:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_HARDROCK);
        renderConnectedTile8(x, y, 416, 64);
        break;
    case TILE_DIRT: // render dots.
        if (level > 1)
            tileRender16(x, y, 320, 80, 0);
        else
            tileRender16(x, y, 336, 80, 0);
        break;
    case TILE_SAND:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAND);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CACTUS);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_SAPLING_CACTUS);
        
        if(level==1 && worldData.season==3) {
            renderConnectedTile4(x, y, 256, 112);
        } else {
            renderConnectedTile4(x, y, 320, 0);
        
            if (d > 0) {
                tileRender16(x, y, 336, 48, 0);
            }
        }
        break;
    case TILE_WATER:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_WATER);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_HOLE);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_ICE);
        
        renderConnectedTile4(x, y, 384, 0);
        
        srand((syncTickCount + (x / 2 - y) * 4311) / 10);
        renderDots(x, y, rand() & 3, rand() & 3, rand() & 3, rand() & 3, 288, 64);
        break;
    case TILE_LAVA:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_LAVA);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_HOLE);
        
        renderConnectedTile4(x, y, 448, 0);
        
        srand((syncTickCount + (x / 2 - y) * 4311) / 10);
        renderDots(x, y, rand() & 3, rand() & 3, rand() & 3, rand() & 3, 304, 64);
        break;
    case TILE_HOLE:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_HOLE);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_WATER);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_LAVA);
        
        renderConnectedTile4(x, y, 256, 16);
        break;
    case TILE_CACTUS:
        renderTile(TILE_SAND, 0, level, x, y);
        tileRender16(x, y, 304, 48, 0);
        break;
    case TILE_FLOWER:
        renderTile(TILE_GRASS, 0, level, x, y);
        if(level==1 && worldData.season==3) tileRender16(x, y, 320, 112, d);
        else tileRender16(x, y, 320, 48, d);
        break;
    case TILE_STAIRS_DOWN:
        if (level == 0)
            renderTile(TILE_CLOUD, 0, level, x, y);
        tileRender16(x, y, 256, 64, 0);
        break;
    case TILE_STAIRS_UP:
        tileRender16(x, y, 272, 64, 0);
        break;
    case TILE_IRONORE:
        tileRender16(x, y, 464, 48, 0);
        break;
    case TILE_GOLDORE:
        tileRender16(x, y, 480, 48, 0);
        break;
    case TILE_GEMORE:
        tileRender16(x, y, 496, 48, 0);
        break;
    case TILE_CLOUD:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CLOUD);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_STAIRS_DOWN);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_CLOUDCACTUS);
        
        renderConnectedTile4(x, y, 320, 16);
        break;
    case TILE_CLOUDCACTUS:
        renderTile(TILE_CLOUD, 0, level, x, y);
        tileRender16(x, y, 496, 64, 0);
        break;
    case TILE_SAPLING_TREE:
        renderTile(TILE_GRASS, 0, level, x, y);
        tileRender16(x, y, 288, 48, 0);
        break;
    case TILE_SAPLING_CACTUS:
        renderTile(TILE_SAND, 0, level, x, y);
        tileRender16(x, y, 288, 48, 0);
        break;
    case TILE_FARM:
        tileRender16(x, y, 352, 48, 0);
        break;
    case TILE_WHEAT:
        age = d / 20;
        if (age > 5)
            age = 5;
        tileRender16(x, y, 368 + (age << 4), 48, 0);
        break;
    case TILE_WOOD_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_WOOD_WALL);
        
        renderConnectedTile4(x, y, 384, 16);
        break;
    case TILE_STONE_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_STONE_WALL);
        
        renderConnectedTile4(x, y, 256, 80);
        break;
    case TILE_IRON_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_IRON_WALL);
        
        renderConnectedTile4(x, y, 448, 16);
        break;
    case TILE_GOLD_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GOLD_WALL);
        
        renderConnectedTile4(x, y, 256, 32);
        break;
    case TILE_GEM_WALL:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_GEM_WALL);
        
        renderConnectedTile4(x, y, 320, 32);
        break;
    case TILE_DUNGEON_WALL:
        checkSurrTiles8(level, x >> 4, y >> 4, TILE_DUNGEON_WALL);
        
        renderConnectedTile8(x, y, 384, 32);
        break;
    case TILE_DUNGEON_FLOOR:
        tileRender16(x, y, 464 + d*16, 32, 0);
        break;
    case TILE_DUNGEON_ENTRANCE:
        tileRender16(x, y, 352 + (level==5 ? 16 : 0), 80, 0);
        break;
    case TILE_MAGIC_BARRIER:
        renderTile(TILE_DUNGEON_FLOOR, 0, level, x, y);
        tileRender16(x, y, 320, 64, d);
        
        //draw remaining pillar count
        PlayerData *lp = getLocalPlayer();
        if((lp->entity.x - (x+8))*(lp->entity.x - (x+8)) + (lp->entity.y - (y+8))*(lp->entity.y - (y+8)) <= 24*24) {
            x -= offsetX;
            y -= offsetY;
        
            int data = 0;
            int i = 0;
            for (i = 0; i < eManager.lastSlot[level]; ++i) {
                Entity * e = &eManager.entities[level][i];
            
                if(e->type == ENTITY_MAGIC_PILLAR) {
                    ++data;
                }
            }
            
            char currentCount[3];
            sprintf(currentCount, "%d", data);
            
            renderTextColorSized(currentCount, x+4 + 1, y+4 + 1, 2, dungeonColor[1]);
            renderTextColorSized(currentCount, x+4, y+4, 2, dungeonColor[0]);
        }
        
        break;
    case TILE_BOOKSHELVES:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_BOOKSHELVES);
        
        renderConnectedTile4(x, y, 384, 80 + d*16);
        break;
    case TILE_WOOD_FLOOR:
        tileRender16(x, y, 336, 96, 0);
        break;
    case TILE_MYCELIUM:
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MYCELIUM);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MUSHROOM_BROWN);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_MUSHROOM_RED);
        
        if(level==1 && worldData.season==3) renderConnectedTile4(x, y, 256, 112);
        else renderConnectedTile4(x, y, 448, 80);
        break;
    case TILE_MUSHROOM_BROWN:
        renderTile(TILE_MYCELIUM, 0, level, x, y);
        tileRender16(x, y, 448 + (d&0x1)*16, 96, 0);
        break;
    case TILE_MUSHROOM_RED:
        renderTile(TILE_MYCELIUM, 0, level, x, y);
        tileRender16(x, y, 480 + (d&0x1)*16, 96, 0);
        break;
    case TILE_ICE:
        renderTile(TILE_WATER, 0, level, x, y);
        //checkSurrTiles4(x >> 4, y >> 4, TILE_WATER);
        //checkSurrTiles4(x >> 4, y >> 4, TILE_HOLE);
        checkSurrTiles4(level, x >> 4, y >> 4, TILE_ICE);
        
        renderConnectedTile4(x, y, 448, 112);
        break;
    }

    resetSurrTiles();
}

void renderConnectedTile4(int x, int y, u32 xTile, u32 yTile) {
    //render complete tile in one piece to reduce strain(added for o3DS)
    if (tl && tr && tu && td) {
        tileRender16(x, y, xTile+48, yTile, 0);
        return;
    }
    
    int l = (tl ? 16 : 0);
    int r = (tr ? 16 : 0);
    int u = (tu ? 32 : 0);
    int d = (td ? 32 : 0);
    
    tileRender8(x,   y,   xTile  +l+u, yTile, 0);
    tileRender8(x+8, y,   xTile+8+r+u, yTile, 0);
    tileRender8(x,   y+8, xTile  +l+d, yTile+8, 0);
    tileRender8(x+8, y+8, xTile+8+r+d, yTile+8, 0);
}

void renderConnectedTile8(int x, int y, u32 xTile, u32 yTile) {
    //render complete tile in one piece to reduce strain(added for o3DS)
    if (tl && tr && tu && td && tul && tur && tdl && tdr) {
        tileRender16(x, y, xTile+64, yTile, 0);
        return;
    }
    
    int l = (tl ? 16 : 0);
    int r = (tr ? 16 : 0);
    int u = (tu ? 32 : 0);
    int d = (td ? 32 : 0);
    
    tileRender8(x,   y,   xTile  +l+u+((tl && tu && tul) ? 16 : 0), yTile, 0);
    tileRender8(x+8, y,   xTile+8+r+u+((tr && tu && tur) ? 16 : 0), yTile, 0);
    tileRender8(x,   y+8, xTile  +l+d+((tl && td && tdl) ? 16 : 0), yTile+8, 0);
    tileRender8(x+8, y+8, xTile+8+r+d+((tr && td && tdr) ? 16 : 0), yTile+8, 0);
}

void renderBackground(s8 level, int xScroll, int yScroll) {
    if(level == 0) {
        sf2d_draw_texture_part_scale(minimap[1], (-xScroll / 3) - 256, (-yScroll / 3) - 32, 0, 0, 128, 128, 12.5, 7.5);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xAFDFDFDF);
    } else if(level == 5) {
        sf2d_draw_rectangle(0, 0, 400, 240, dungeonColor[1]);
    } else {
        sf2d_draw_rectangle(0, 0, 400, 240, dirtColor[level]); // dirt color
    }
    
    batch_start();
    inBatch = true;
    int xo = xScroll >> 4;
    int yo = yScroll >> 4;
    int x, y;
    for (x = xo; x <= 13 + xo; ++x) {
        for (y = yo; y <= 8 + yo; ++y)
            renderTile(getTile(level, x, y), getData(level, x, y), level, x << 4, y << 4);
    }
    inBatch = false;
    batch_end();
}
