#pragma once

#include "Data.h"
#include "Input.h"
#include "Entity.h"
#include "QuestsData.h"
#include "Crafting.h"

#define MAX_PLAYERS 8
#define MAX_INPUT_BUFFER 3

#define PLAYER_SPRITE_HEAD_COUNT 5
#define PLAYER_SPRITE_EYES_COUNT 5
#define PLAYER_SPRITE_BODY_COUNT 7
#define PLAYER_SPRITE_ARMS_COUNT 7
#define PLAYER_SPRITE_LEGS_COUNT 6
#define PLAYER_SPRITE_ACCS_COUNT 6

#define PLAYER_SPRITE_ACCS_FRONT { true, true, false, false, true, true }

//TODO: Implement !GUI(with armor and effects, next to inventory?)!

typedef struct _plrsp {
    bool choosen;
    
    u8 legs;
    u8 body;
    u8 arms;
    u8 head;
    u8 eyes;
    u8 accs;
} PlayerSprite;

typedef struct _plrefx {
    u8 level;
    u32 time;
} PlayerEffect;

typedef struct _plrd {
    //for identification in save data and sync game start
    u32 id;
    bool idSet;
    bool ready;
    
    //input/multiplayer/synchronization
    Inputs inputs;
    Inputs nextInputs[MAX_INPUT_BUFFER];
    bool nextTurnReady[MAX_INPUT_BUFFER];
    
    //
    bool isSpawned;
    u8 minimapData[128*128];
    
    int score;
    QuestlineManager questManager;
    
    Entity entity;
    Inventory inventory;
    Item *activeItem;
    
    PlayerEffect effects[EFFECTS_MAX];
    
    PlayerSprite sprite;
    
    //menu data
    u8 ingameMenu;
    s8 ingameMenuSelection;
    s16 ingameMenuInvSel;
    s16 ingameMenuInvSelOther;
    bool ingameMenuAreYouSure;
    bool ingameMenuAreYouSureSave;
    s16 ingameMenuTimer;
    NPC_MenuData npcMenuData;
    
    RecipeManager currentRecipes;
    char *currentCraftTitle;
    Entity *curChestEntity;
    s8 curChestEntityR;
    
    bool mapShouldRender;
    u8 mapZoomLevel;
    s16 mapScrollX;
    s16 mapScrollY;
    char mapText[32];
    
    s16 touchLastX;
    s16 touchLastY;
    bool touchIsDraggingMap;
    bool touchIsChangingSize;
} PlayerData;

PlayerData players[MAX_PLAYERS];
int playerCount;
int playerLocalID;

void initPlayers();
void freePlayers();

void initPlayer(PlayerData *pd);
void freePlayer(PlayerData *pd);

PlayerData* getNearestPlayer(s8 level, s16 x, s16 y);
PlayerData* getLocalPlayer();

void tickPlayer(PlayerData *pd, bool inmenu);
void playerSetActiveItem(PlayerData *pd, Item * item);
bool playerUseEnergy(PlayerData *pd, int amount);
void playerHeal(PlayerData *pd, int amount);
void playerDamage(PlayerData *pd, int damage, int dir, u32 hurtColor, Entity *damager);
void playerSpawn(PlayerData *pd);

// effects
void playerEffectsUpdate(PlayerData *pd);
bool playerEffectActive(PlayerData *pd, int effect);
void playerEffectApply(PlayerData *pd, int effect, u8 level, u32 time);
void playerEffectRemove(PlayerData *pd, int effect);
u8 playerEffectGetLevel(PlayerData *pd, int effect);
u32 playerEffectGetTime(PlayerData *pd, int effect);
