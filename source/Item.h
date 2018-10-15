#pragma once
#include <3ds.h>
#include <sf2d.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Sound.h"

typedef struct Inventory Inventory;

typedef struct {
    s16 id; 
    s16 countLevel; // Count for items, Level for tools.
    s16 slotNum; // Read-only. Do not mess with this.
    int* invPtr; // pointer to current inventory.
    Inventory * chestPtr; // pointer to chest inventory for chest item.
} Item;

struct Inventory {
    Item items[300]; // Maximum of 300 slots in every inventory.
    s16 lastSlot; // lastSlot can also be used to read the size of the inventory.
};

bool isItemEmpty(Item* item);
Item newItem(int id, int cLevel);
void pushItemToInventoryFront(Item item, Inventory * inv);
void addItemToInventory(Item item, Inventory * inv);
void removeItemFromCurrentInv(Item* item);
void removeItemFromInventory(int slot, Inventory * inv);
Item* getItemFromInventory(int itemID, Inventory * inv);
int countItemInv(int itemID,int level, Inventory* inv);
