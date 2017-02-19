#pragma once
#include <3ds.h>
#include "SaveLoad.h"
#include "Input.h"
#include "MapGen.h"
#include "Quests.h"

#include "icons2_png.h"
#include "Font_png.h"
#include "bottombg_png.h"

#define CIRCLEPAD 0xF0000000
#define CSTICK 0x0F000000

#define MENU_NONE 0
#define MENU_TITLE 1
#define MENU_TUTORIAL 2
#define MENU_ABOUT 3
#define MENU_SETTINGS 4
#define MENU_INVENTORY 5
#define MENU_CRAFTING 6
#define MENU_CONTAINER 7
#define MENU_WIN 8
#define MENU_LOSE 9
#define MENU_PAUSED 10
#define MENU_LOADGAME 11
#define MENU_SETTINGS_REBIND 12
#define MENU_SETTINGS_TP 13
#define MENU_DUNGEON 14
#define MENU_NPC 15
#define MENU_MULTIPLAYER 16

#define NPC_GIRL 0
#define NPC_PRIEST 1
#define NPC_FARMER 2
#define NPC_LIBRARIAN 3
#define NPC_DWARF 4

#define TILE_NULL 255
#define TILE_GRASS 0
#define TILE_TREE 1
#define TILE_ROCK 2
#define TILE_DIRT 3
#define TILE_SAND 4
#define TILE_WATER 5
#define TILE_LAVA 6
#define TILE_CACTUS 7
#define TILE_FLOWER 8
#define TILE_IRONORE 9
#define TILE_GOLDORE 10
#define TILE_GEMORE 11
#define TILE_FARM 12
#define TILE_WHEAT 13
#define TILE_SAPLING_TREE 14
#define TILE_SAPLING_CACTUS 15
#define TILE_STAIRS_DOWN 16
#define TILE_STAIRS_UP 17
#define TILE_CLOUD 18
#define TILE_HARDROCK 19
#define TILE_CLOUDCACTUS 20
#define TILE_HOLE 21

#define TILE_WOOD_WALL 22
#define TILE_STONE_WALL 23
#define TILE_IRON_WALL 24
#define TILE_GOLD_WALL 25
#define TILE_GEM_WALL 26
#define TILE_DUNGEON_WALL 27
#define TILE_DUNGEON_FLOOR 28
#define TILE_DUNGEON_ENTRANCE 29
#define TILE_MAGIC_BARRIER 30
#define TILE_BOOKSHELVES 31
#define TILE_WOOD_FLOOR 32
#define TILE_MYCELIUM 33
#define TILE_MUSHROOM_BROWN 34
#define TILE_MUSHROOM_RED 35
#define TILE_ICE 36

#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

//TODO: Dont forget to change back
#define TESTGODMODE true

bool screenShot;
int loadedtp;

u8 MODEL_3DS;

extern char versionText[34];

Entity player;

bool shouldRenderDebug;
bool shouldSpeedup;
bool shouldRenderMap;
u8 zoomLevel;
char mapText[32];
s16 mScrollX, mScrollY;

sf2d_texture *icons;
sf2d_texture *font;
sf2d_texture *bottombg;
sf2d_texture * minimap[6];
u8 map[6][128*128];
u8 data[6][128*128];
u8 minimapData[128*128];
u8 compassData[6][3];

u32 dirtColor[5];
u32 grassColor;
u32 sandColor;
u32 waterColor[2];
u32 lavaColor[2];
u32 rockColor[4];
u32 woodColor;
u32 ironColor;
u32 goldColor;
u32 gemColor;
u32 dungeonColor[2];
u32 myceliumColor;
u32 mushroomColor;
u32 snowColor;
u32 iceColor;

char currentFileName[256];
extern u8 currentMenu;
extern char fpsstr[];
u8 initGame;
u8 initBGMap;
Item noItem;
int airWizardHealthDisplay;
s16 awX, awY;
u32 tickCount;
RecipeManager* currentRecipes;
Entity* curChestEntity;
char* currentCraftTitle;
s16 curInvSel;
bool quitGame;
s8 currentSelection;
bool isRemote;

u16 daytime;
int day;
u8 season;
bool rain;

void tickTile(int x, int y);
bool tileIsSolid(int tile, Entity * e);

s8 itemTileInteract(int tile, Item* item, int x, int y, int px, int py, int dir);

void tickEntity(Entity* e);

void tickTouchMap();
void tickTouchQuickSelect();

void trySpawn(int count, int level);

int getTile(int x, int y);
u32 getTileColor(int tile);
void setTile(int id, int x, int y);
int getData(int x, int y);
void setData(int id, int x, int y);

bool intersectsEntity(int x, int y, int r, Entity* e);

bool EntityBlocksEntity(Entity* e1, Entity* e2);
void EntityVsEntity(Entity* e1, Entity* e2);
void entityTileInteract(Entity* e, int tile,int x, int y);

void initPlayer();
void tickPlayer();
void playerAttack();
bool isSwimming();
bool playerUseEnergy(int amount);
void playerHurtTile(int tile, int xt, int yt, int damage, int dir);
bool playerIntersectsEntity(Entity* e);
void playerEntityInteract(Entity* e);
void playerSetActiveItem(Item * item);

void enterDungeon();
void leaveDungeon();

void setMinimapVisible(int level, int x, int y, bool visible);
bool getMinimapVisible(int level, int x, int y);
u32 getMinimapColor(int level, int x, int y);
void initMinimapLevel(int level, bool loadUpWorld);
void updateLevel1Map();

void reloadColors();