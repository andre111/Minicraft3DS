#pragma once
#include "engine/engine.h"

#include "Data.h"

#include "Entity.h"
#include "Inputs.h"
#include "MapGen.h"
#include "Menu.h"
#include "Player.h"
#include "Quests.h"
#include "Sound.h"
#include "network/Synchronizer.h"

#include <stdio.h>
#include <string.h>

#define CIRCLEPAD 0xF0000
#define CSTICK 0xF00000

#define MENU_PAUSED 100
#define MENU_INVENTORY 101
#define MENU_CRAFTING 102
#define MENU_CONTAINER 103
#define MENU_WIN 104
#define MENU_LOSE 105
#define MENU_DUNGEON 106
#define MENU_NPC 107
#define MENU_CHARACTER_CUSTOMIZE 108

#define NPC_GIRL 0
#define NPC_PRIEST 1
#define NPC_FARMER 2
#define NPC_LIBRARIAN 3
#define NPC_DWARF 4

// WARNING: Having this set to different values in different clients will break multiplayer!
// TODO: Change back before any release
#define TESTGODMODE false

extern sInt localUID;

extern int loadedtp;

extern char versionText[34];

extern bool shouldRenderDebug;

extern MImage imageIcons;
extern MImage imagePlayerSprites;
extern MImage imageFont;
extern MImage imageBottombg;
extern MImage imageMinimap[6];

extern char currentFileName[256];
extern uByte currentMenu;
extern char fpsstr[];
extern uByte initGame;
extern uByte initMPGame;
extern uByte initBGMap;
extern Item noItem;
extern int airWizardHealthDisplay;
extern sShort awX, awY;
extern bool quitGame;
extern int currentSelection;

typedef struct _worldData {
    uByte map[6][128 * 128];
    uByte data[6][128 * 128];

    uShort daytime;
    int day;
    uByte season;
    bool rain;

    uByte compassData[6][3];
} WorldData;

extern WorldData worldData;

extern int syncTickCount;

// TODO: cleanup the order
extern void addItemsToWorld(Item item, uByte level, int x, int y, int count);
extern bool intersects(Entity e, int x0, int y0, int x1, int y1);
extern int getEntities(Entity **result, uByte level, int x0, int y0, int x1, int y1);

// TODO: the move functions should probably be part of entity.c
extern bool move(Entity *e, int xa, int ya);
extern bool moveMob(Entity *e, int xa, int ya);
extern void hurtEntity(Entity *e, int damage, int dir, MColor hurtColor, Entity *damager);

extern void tickTile(uByte level, int x, int y);
extern bool tileIsSolid(int tile, Entity *e);

extern int itemTileInteract(int tile, PlayerData *pd, Item *item, uByte level, int x, int y, int px, int py, int dir);

extern void trySpawn(int count, uByte level);

extern int getTile(uByte level, int x, int y);
extern void setTile(int id, uByte level, int x, int y);
extern int getData(uByte level, int x, int y);
extern void setData(int id, uByte level, int x, int y);
extern MColor getTileColor(int tile);

extern bool intersectsEntity(int x, int y, int r, Entity *e);

extern bool EntityBlocksEntity(Entity *e1, Entity *e2);
extern void EntityVsEntity(Entity *e1, Entity *e2);
extern bool ItemVsEntity(PlayerData *pd, Item *item, Entity *e, int dir);
extern void entityTileInteract(Entity *e, int tile, uByte level, int x, int y);

extern void openCraftingMenu(PlayerData *pd, RecipeManager *rm, char *title);
extern bool useEntity(PlayerData *pd, Entity *e);

extern bool isWater(uByte level, int xt, int yt);

extern void playerHurtTile(PlayerData *pd, int tile, uByte level, int xt, int yt, int damage, int dir);
extern void playerEntityInteract(PlayerData *pd, Entity *e);

extern bool dungeonActive();
extern void enterDungeon(PlayerData *pd);
extern void leaveDungeon(PlayerData *pd);

extern void setMinimapVisible(PlayerData *pd, uByte level, int x, int y, bool visible);
extern bool getMinimapVisible(PlayerData *pd, uByte level, int x, int y);
extern MColor getMinimapColor(PlayerData *pd, uByte level, int x, int y);
extern void initMinimapLevel(PlayerData *pd, uByte level);
extern void updateLevel1Map();
