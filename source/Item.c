#include "Item.h"

#include "Data.h"

bool isItemEmpty(Item* item) {
    if(itemIsSingle(item->id, item->countLevel)) return false;
    if(item->countLevel < 1) return true;
    return false;
}

void pushItemToInventoryFront(Item item, Inventory * inv) {
    if(inv->lastSlot < 300) ++inv->lastSlot;
    int i;
    for(i = inv->lastSlot;i > 0;--i) { 
        inv->items[i] = inv->items[i-1]; // Move the items up the list.
        inv->items[i].slotNum = i;
    }
    item.invPtr = (int*)inv;
    inv->items[0] = item;
    inv->items[0].slotNum = 0;
}

void addItemToInventory(Item item, Inventory * inv) {
    if(!itemIsSingle(item.id, item.countLevel)) {
        int i;
        for(i = 0; i < inv->lastSlot; ++i) { //Search inventory if item already exists.
            if(inv->items[i].id == item.id) {
                inv->items[i].countLevel += item.countLevel;
                return;
            }
        }
    }
    
    item.slotNum = inv->lastSlot;
    item.invPtr = (int*)inv;
    inv->items[inv->lastSlot] = item;
    ++inv->lastSlot;
}

void removeItemFromCurrentInv(Item* item) {
    removeItemFromInventory(item->slotNum, (Inventory*)item->invPtr);
}

Item nullItem;
void removeItemFromInventory(int slot, Inventory * inv) {
    int i;
    for(i = slot;i < inv->lastSlot - 1;++i) { 
        inv->items[i] = inv->items[i + 1]; // Move the items down.
        inv->items[i].slotNum--;
    }
    --inv->lastSlot;
    inv->items[inv->lastSlot] = nullItem; // Make the last slot null.
}

Item newItem(int id, int cLevel) {
    Item item;
    item.id = id;
    if(id != ITEM_NULL){
        if(cLevel > 999) cLevel = 999;
        item.countLevel = cLevel;
    }
    item.chestPtr = NULL;
    return item;
}

Item* getItemFromInventory(int itemID, Inventory * inv) {
    int i;
    for(i = 0;i < inv->lastSlot;++i){ 
        if(inv->items[i].id == itemID){
            return &inv->items[i];
        }
    }
    return (Item*)NULL;
}

int countItemInv(int itemID, int level, Inventory* inv) {
    int i, count = 0;
    for(i = 0;i < inv->lastSlot;++i){ 
        if(inv->items[i].id == itemID){ 
            if(itemIsSingle(inv->items[i].id, inv->items[i].countLevel)) {
                if(level == inv->items[i].countLevel) count++;
            } else {
                count += inv->items[i].countLevel;
            }
        }
    }
    return count;
}
