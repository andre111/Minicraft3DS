#include "../Entity.h"

#include "../Data.h"

void assignInventory(Entity* e) {
    if(eManager.nextInv > 300) return;
    e->entityFurniture.inv = &eManager.invs[eManager.nextInv];
    eManager.nextInv++;
}

Entity newEntityFurniture(int itemID, Inventory * invPtr, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_FURNITURE;
    e.level = level;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.entityFurniture.itemID = itemID;
    e.canPass = false;
    if(itemID == ITEM_LANTERN) {
        e.entityFurniture.r = 8;
    } else if(itemID == ITEM_CHEST) { //TODO: This should not happen here, the new functions should not mess with any outside state
        if(invPtr == NULL) assignInventory(&e);
        else e.entityFurniture.inv = invPtr;
    }
    
    e.tickFunction = NULL;
    
    return e;
}
