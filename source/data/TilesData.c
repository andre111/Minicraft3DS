#include "TilesData.h"

static char* _tileNames[MAX_TILE_ID+1];

static void _tileRegister(u8 id, char* name) {
    _tileNames[id] = name;
}

void tilesDataInit() {
    _tileRegister(TILE_NULL, "");
    _tileRegister(TILE_GRASS, "Grass");
    _tileRegister(TILE_TREE, "Tree");
    _tileRegister(TILE_ROCK, "Rock");
    _tileRegister(TILE_DIRT, "Dirt");
    _tileRegister(TILE_SAND, "Sand");
    _tileRegister(TILE_WATER, "Water");
    _tileRegister(TILE_LAVA, "Lava");
    _tileRegister(TILE_CACTUS, "Cactus");
    _tileRegister(TILE_FLOWER, "Flower");
    _tileRegister(TILE_IRONORE, "Iron Ore");
    _tileRegister(TILE_GOLDORE, "Gold Ore");
    _tileRegister(TILE_GEMORE, "Gem Ore");
    _tileRegister(TILE_FARM, "Farm");
    _tileRegister(TILE_WHEAT, "Wheat");
    _tileRegister(TILE_SAPLING_TREE, "Tree Sapling");
    _tileRegister(TILE_SAPLING_CACTUS, "Cactus Sapling");
    _tileRegister(TILE_STAIRS_DOWN, "Stairs Down");
    _tileRegister(TILE_STAIRS_UP, "Stairs Up");
    _tileRegister(TILE_CLOUD, "Cloud");
    _tileRegister(TILE_HARDROCK, "Hard Rock");
    _tileRegister(TILE_CLOUDCACTUS, "Cloud Cactus");
    _tileRegister(TILE_HOLE, "Hole");

    _tileRegister(TILE_WOOD_WALL, "Wood Wall");
    _tileRegister(TILE_STONE_WALL, "Stone Wall");
    _tileRegister(TILE_IRON_WALL, "Iron Wall");
    _tileRegister(TILE_GOLD_WALL, "Gold Wall");
    _tileRegister(TILE_GEM_WALL, "Gem Wall");
    _tileRegister(TILE_DUNGEON_WALL, "Dungeon Wall");
    _tileRegister(TILE_DUNGEON_FLOOR, "Dungeon Floor");
    _tileRegister(TILE_DUNGEON_ENTRANCE, "Dungeon Entrance");
    _tileRegister(TILE_MAGIC_BARRIER, "Magic Barrier");
    _tileRegister(TILE_BOOKSHELVES, "Bookshelves");
    _tileRegister(TILE_WOOD_FLOOR, "Wood Floor");
    _tileRegister(TILE_MYCELIUM, "Mycelium");
    _tileRegister(TILE_MUSHROOM_BROWN, "Brown Mushroom");
    _tileRegister(TILE_MUSHROOM_RED, "Red Mushroom");
    _tileRegister(TILE_ICE, "Ice");
}


char* tileGetName(u8 id) {
    return _tileNames[id];
}
