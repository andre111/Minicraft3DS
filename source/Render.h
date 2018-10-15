#pragma once

#include "render/RenderText.h"
#include "render/RenderTiles.h"
#include "render/RenderLights.h"

#include <3ds.h>
#include <sf2d.h>
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#include "Globals.h"
#include "MapGen.h"

int offsetX, offsetY;
int playerScale;

void render(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);
void renderb(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color);
void renderr(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float rotate);
void renderc(s32 xp, s32 yp, u32 xTile, u32 yTile, int sizeX, int sizeY, u8 bits);
void render16(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);
void render16c(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float scaleX, float scaleY);
void render16b(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color);
void render32(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits);

void renderTitle(int x, int y);
void renderFrame(int x1, int y1, int x2, int y2, u32 bgColor);

void renderWeather(u8 level, int xScroll, int yScroll);
void renderDayNight(PlayerData *pd);
void renderButtonIcon(u32 icon, int x, int y, float scale);

void renderGui(PlayerData *pd);
void renderZoomedMap(PlayerData *pd);
void renderPlayer(PlayerData *pd);

void renderFurniture(int itemID, int x, int y);
void renderEntity(Entity* e, int x, int y);
void renderEntities(u8 level, int x, int y, EntityManager* em);

void renderRecipes(RecipeManager * r, int xo, int yo, int x1, int y1, int selected);
void renderItemList(Inventory * inv, int xo, int yo, int x1, int y1, int selected);
void renderItemWithText(Item* item, int x, int y);
void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y);
void renderItemWithTextCentered(Item* item, int width, int y);
void renderItemIcon(int itemID, int countLevel, int x, int y);
void renderItemIcon2(int itemID, int countLevel, int x, int y, int z);
