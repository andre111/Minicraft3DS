#pragma once
#include <3ds.h>
#include <sf2d.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "Sound.h"

#define ITEM_NULL 0
#define TOOL_SHOVEL 1
#define TOOL_HOE 2
#define TOOL_SWORD 3
#define TOOL_PICKAXE 4
#define TOOL_AXE 5
#define ITEM_POWGLOVE 6
#define ITEM_FLOWER 7
#define ITEM_WOOD 8
#define ITEM_STONE 9
#define ITEM_SAND 10
#define ITEM_DIRT 11
#define ITEM_CLOUD 12
#define ITEM_ACORN 13
#define ITEM_CACTUS 14
#define ITEM_SEEDS 15
#define ITEM_WHEAT 16
#define ITEM_FLESH 17
#define ITEM_BREAD 18
#define ITEM_APPLE 19
#define ITEM_SLIME 20
#define ITEM_COAL 21
#define ITEM_IRONORE 22
#define ITEM_GOLDORE 23
#define ITEM_IRONINGOT 24
#define ITEM_GOLDINGOT 25
#define ITEM_GLASS 26
#define ITEM_GEM 27
#define ITEM_ANVIL 28
#define ITEM_CHEST 29
#define ITEM_OVEN 30
#define ITEM_FURNACE 31
#define ITEM_WORKBENCH 32
#define ITEM_LANTERN 33

#define ITEM_LOOM 34
#define ITEM_ENCHANTER 35

#define ITEM_WALL_WOOD 51
#define ITEM_WALL_STONE 52
#define ITEM_WALL_IRON 53
#define ITEM_WALL_GOLD 54
#define ITEM_WALL_GEM 55
#define ITEM_WOOL 56
#define ITEM_STRING 57
#define ITEM_PORK_RAW 58
#define ITEM_PORK_COOKED 59
#define ITEM_BEEF_RAW 60
#define ITEM_BEEF_COOKED 61
#define ITEM_LEATHER 62
#define ITEM_ARROW_WOOD 63
#define ITEM_ARROW_STONE 64
#define ITEM_ARROW_IRON 65
#define ITEM_ARROW_GOLD 66
#define ITEM_ARROW_GEM 67
#define ITEM_BONE 68
#define ITEM_DUNGEON_KEY 69
#define ITEM_WIZARD_SUMMON 70
#define ITEM_DRAGON_EGG 71
#define ITEM_DRAGON_SCALE 72
#define ITEM_BOOKSHELVES 73
#define ITEM_MAGIC_DUST 74
#define ITEM_COIN 75

#define TOOL_BUCKET 101
#define TOOL_BOW 102
#define TOOL_MAGIC_COMPASS 103

typedef struct Inventory Inventory;

typedef struct {
    s16 id; 
    s16 countLevel; // Count for items, Level for tools.
    s16 slotNum; // Read-only. Do not mess with this.
    bool onlyOne;
    int* invPtr; // pointer to current inventory.
    Inventory * chestPtr; // pointer to chest inventory for chest item.
} Item;

struct Inventory {
    Item items[300]; // Maximum of 300 slots in every inventory.
    s16 lastSlot; // lastSlot can also be used to read the size of the inventory.
};

bool isItemEmpty(Item* item);
Item newItem(int id, int cLevel);
char* getItemName(int itemID, int countLevel);
char* getBasicItemName(int itemID, int countLevel);
void pushItemToInventoryFront(Item item, Inventory * inv);
void addItemToInventory(Item item, Inventory * inv);
void removeItemFromCurrentInv(Item* item);
void removeItemFromInventory(int slot, Inventory * inv);
Item* getItemFromInventory(int itemID, Inventory * inv);
int countItemInv(int itemID,int level, Inventory* inv);
