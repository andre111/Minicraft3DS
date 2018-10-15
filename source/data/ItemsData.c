#include "ItemsData.h"

#include <stdio.h>

static char* _itemNames[MAX_ITEM_ID+1];
static int _itemIconX[MAX_ITEM_ID+1];
static int _itemIconY[MAX_ITEM_ID+1];
static bool _itemSingle[MAX_ITEM_ID+1];

static void _itemRegister(s16 id, char* name, int iconX, int iconY, bool isSingle) {
    _itemNames[id] = name;
    _itemIconX[id] = iconX;
    _itemIconY[id] = iconY;
    _itemSingle[id] = isSingle;
}

void itemsDataInit() {
    _itemRegister(ITEM_NULL, "", 0, 0, true);
    
    //tools 1
    _itemRegister(TOOL_SHOVEL, "Shovel", 0, 144, true);
    _itemRegister(TOOL_HOE, "Hoe", 40, 144, true);
    _itemRegister(TOOL_SWORD, "Sword", 80, 144, true);
    _itemRegister(TOOL_PICKAXE, "Pickaxe", 120, 144, true);
    _itemRegister(TOOL_AXE, "Axe", 160, 144, true);
    _itemRegister(ITEM_POWGLOVE, "Power Glove", 56, 152, true);

    //items 1
    _itemRegister(ITEM_FLOWER, "Flower", 0, 152, false);
    _itemRegister(ITEM_WOOD, "Wood", 8, 152, false);
    _itemRegister(ITEM_STONE, "Stone", 16, 152, false);
    _itemRegister(ITEM_SAND, "Sand", 216, 152, false);
    _itemRegister(ITEM_DIRT, "Dirt", 224, 152, false);
    _itemRegister(ITEM_CLOUD, "Cloud", 232, 152, false);
    _itemRegister(ITEM_ACORN, "Acorn", 24, 152, false);
    _itemRegister(ITEM_CACTUS, "Cactus", 32, 152, false);
    _itemRegister(ITEM_SEEDS, "Seeds", 40, 152, false);
    _itemRegister(ITEM_WHEAT, "Wheat", 48, 152, false);
    _itemRegister(ITEM_FLESH, "Flesh", 64, 152, false);
    _itemRegister(ITEM_BREAD, "Bread", 72, 152, false);
    _itemRegister(ITEM_APPLE, "Apple", 80, 152, false);
    _itemRegister(ITEM_SLIME, "Slime", 248, 144, false);
    _itemRegister(ITEM_COAL, "Coal", 240, 144, false);
    _itemRegister(ITEM_IRONORE, "Iron Ore", 88, 152, false);
    _itemRegister(ITEM_GOLDORE, "Gold Ore", 240, 152, false);
    _itemRegister(ITEM_IRONINGOT, "Iron Ingot", 96, 152, false);
    _itemRegister(ITEM_GOLDINGOT, "Gold Ingot", 248, 152, false);
    _itemRegister(ITEM_GLASS, "Glass", 104, 152, false);
    _itemRegister(ITEM_GEM, "Gem", 112, 152, false);

    //furniture
    _itemRegister(ITEM_ANVIL, "Anvil", 120, 152, true);
    _itemRegister(ITEM_CHEST, "Chest", 128, 152, true);
    _itemRegister(ITEM_OVEN, "Oven", 136, 152, true);
    _itemRegister(ITEM_FURNACE, "Furnace", 144, 152, true);
    _itemRegister(ITEM_WORKBENCH, "Workbench", 152, 152, true);
    _itemRegister(ITEM_LANTERN, "Lantern", 160, 152, true);
    _itemRegister(ITEM_LOOM, "Loom", 120, 160, true);
    _itemRegister(ITEM_ENCHANTER, "Enchanter", 144, 160, true);

    //items 2
    _itemRegister(ITEM_WALL_WOOD, "Wood Wall", 224, 144, false);
    _itemRegister(ITEM_WALL_STONE, "Stone Wall", 224, 160, false);
    _itemRegister(ITEM_WALL_IRON, "Iron Wall", 232, 160, false);
    _itemRegister(ITEM_WALL_GOLD, "Gold Wall", 240, 160, false);
    _itemRegister(ITEM_WALL_GEM, "Gem Wall", 248, 160, false);
    _itemRegister(ITEM_WOOL, "Wool", 64, 160, false);
    _itemRegister(ITEM_STRING, "String", 72, 160, false);
    _itemRegister(ITEM_PORK_RAW, "Raw Pork", 80, 160, false);
    _itemRegister(ITEM_PORK_COOKED, "Cooked Pork", 88, 160, false);
    _itemRegister(ITEM_BEEF_RAW, "Raw Beef", 96, 160, false);
    _itemRegister(ITEM_BEEF_COOKED, "Steak", 104, 160, false);
    _itemRegister(ITEM_LEATHER, "Leather", 112, 160, false);
    _itemRegister(ITEM_ARROW_WOOD, "Wood Arrow", 72, 168, false);
    _itemRegister(ITEM_ARROW_STONE, "Stone Arrow", 80, 168, false);
    _itemRegister(ITEM_ARROW_IRON, "Iron Arrow", 88, 168, false);
    _itemRegister(ITEM_ARROW_GOLD, "Gold Arrow", 96, 168, false);
    _itemRegister(ITEM_ARROW_GEM, "Gem Arrow", 104, 168, false);
    _itemRegister(ITEM_BONE, "Bone", 128, 160, false);
    _itemRegister(ITEM_DUNGEON_KEY, "Dungeon Key", 136, 160, false);
    _itemRegister(ITEM_WIZARD_SUMMON, "Wizard Summon", 152, 160, false);
    _itemRegister(ITEM_DRAGON_EGG, "Dragon Egg", 160, 160, false);
    _itemRegister(ITEM_DRAGON_SCALE, "Dragon Scale", 168, 160, false);
    _itemRegister(ITEM_BOOKSHELVES, "Bookshelves", 232, 144, false);
    _itemRegister(ITEM_MAGIC_DUST, "Magic Dust", 200, 152, false);
    _itemRegister(ITEM_COIN, "Coin", 208, 152, false);

    //tools 2
    _itemRegister(TOOL_BUCKET, "Bucket", 200, 144, true);
    _itemRegister(TOOL_BOW, "Bow", 64, 168, true);
    _itemRegister(TOOL_MAGIC_COMPASS, "Magic Compass", 112, 168, true);

    //items 3
    _itemRegister(ITEM_SCROLL_UNDYING, "Undying", 144, 168, false);
    _itemRegister(ITEM_SCROLL_REGENERATION, "Regeneration", 144, 168, false);
    _itemRegister(ITEM_SCROLL_SPEED, "Speed", 144, 168, false);
    _itemRegister(ITEM_SCROLL_STRENGTH, "Strength", 144, 168, false);
    _itemRegister(ITEM_SCROLL_SHIELDING, "Shielding", 144, 168, false);
    _itemRegister(ITEM_SCROLL_NIGHTVISION, "Night Vision", 144, 168, false);
}


char* itemGetName(s16 id, s16 countLevel) {
    switch(id) {
    //handle special cases here
    case TOOL_SHOVEL:
        switch(countLevel){
            case 1: return "Rock Shovel";
            case 2: return "Iron Shovel";
            case 3: return "Gold Shovel";
            case 4: return "Gem Shovel";
            default: return "Wood Shovel";
        }
    case TOOL_HOE:
        switch(countLevel){
            case 1: return "Rock Hoe";
            case 2: return "Iron Hoe";
            case 3: return "Gold Hoe";
            case 4: return "Gem Hoe";
            default: return "Wood Hoe";
        }
    case TOOL_SWORD:
        switch(countLevel){
            case 1: return "Rock Sword";
            case 2: return "Iron Sword";
            case 3: return "Gold Sword";
            case 4: return "Gem Sword";
            default: return "Wood Sword";
        }
    case TOOL_PICKAXE:
        switch(countLevel){
            case 1: return "Rock Pickaxe";
            case 2: return "Iron Pickaxe";
            case 3: return "Gold Pickaxe";
            case 4: return "Gem Pickaxe";
            default: return "Wood Pickaxe";
        }
    case TOOL_AXE:
        switch(countLevel){
            case 1: return "Rock Axe";
            case 2: return "Iron Axe";
            case 3: return "Gold Axe";
            case 4: return "Gem Axe";
            default: return "Wood Axe";
        }
    case TOOL_BUCKET:
        switch(countLevel){
            case 1: return "Water Bucket";
            case 2: return "Lava Bucket";
            default: return "Empty Bucket";
        }
    //else just return the registered value
    default:
        return _itemNames[id];
    }
}

char _itemCurrentName[20];
char* itemGetNameWithCount(s16 id, s16 countLevel) {
    if(itemIsSingle(id, countLevel)) {
        return itemGetName(id, countLevel);
    } else {
        sprintf(_itemCurrentName, "%d %s", countLevel, itemGetName(id, countLevel));
        return _itemCurrentName;
    }
}

int itemGetIconX(s16 id, s16 countLevel) {
    switch(id) {
    //handle special cases here
    case TOOL_SHOVEL:
    case TOOL_HOE:
    case TOOL_SWORD:
    case TOOL_PICKAXE:
    case TOOL_AXE:
    case TOOL_BUCKET:
        return _itemIconX[id] + countLevel * 8;
    //else just return the registered value
    default:
        return _itemIconX[id];
    }
}

int itemGetIconY(s16 id, s16 countLevel) {
    switch(id) {
    //handle special cases here
    
    //else just return the registered value
    default:
        return _itemIconY[id];
    }
}

bool itemIsSingle(s16 id, s16 countLevel) {
    return _itemSingle[id];
}
