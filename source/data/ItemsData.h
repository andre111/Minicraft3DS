#pragma once

#include <3ds.h>

//NOTE: Items 0-6, 28-50, 101-1000 are for unstackable (tools, furniture) items
#define ITEM_NULL 0

//tools 1
#define TOOL_SHOVEL 1
#define TOOL_HOE 2
#define TOOL_SWORD 3
#define TOOL_PICKAXE 4
#define TOOL_AXE 5
#define ITEM_POWGLOVE 6

//items 1
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

//furniture
#define ITEM_ANVIL 28
#define ITEM_CHEST 29
#define ITEM_OVEN 30
#define ITEM_FURNACE 31
#define ITEM_WORKBENCH 32
#define ITEM_LANTERN 33
#define ITEM_LOOM 34
#define ITEM_ENCHANTER 35

//items 2
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

//tools 2
#define TOOL_BUCKET 101
#define TOOL_BOW 102
#define TOOL_MAGIC_COMPASS 103

//items 3
#define ITEM_SCROLL_UNDYING 1001
#define ITEM_SCROLL_REGENERATION 1002
#define ITEM_SCROLL_SPEED 1003
#define ITEM_SCROLL_STRENGTH 1004
#define ITEM_SCROLL_SHIELDING 1005
#define ITEM_SCROLL_NIGHTVISION 1006


#define MAX_ITEM_ID 1006

void itemsDataInit();

char* itemGetName(s16 id, s16 countLevel);
char* itemGetNameWithCount(s16 id, s16 countLevel);
int itemGetIconX(s16 id, s16 countLevel);
int itemGetIconY(s16 id, s16 countLevel);
bool itemIsSingle(s16 id, s16 countLevel);
