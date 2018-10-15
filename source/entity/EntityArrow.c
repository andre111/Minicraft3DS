#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityArrow(Entity *e, PlayerData *nearestPlayer);

Entity newEntityArrow(Entity* parent, int itemID, s8 xa, s8 ya, int level) {
    Entity e;
    e.type = ENTITY_ARROW;
    e.level = level;
    e.arrow.age = 0;
    e.arrow.parent = parent;
    e.arrow.itemID = itemID;
    e.arrow.xa = xa;
    e.arrow.ya = ya;
    e.x = parent->x;
    e.y = parent->y;
    e.xr = 2;
    e.yr = 2;
    e.canPass = false;
    e.canSwim = true;
    
    e.tickFunction = &tickEntityArrow;
    
    return e;
}

void tickEntityArrow(Entity *e, PlayerData *nearestPlayer) {
    e->arrow.age++;
    if (e->arrow.age >= 260 || !move(e, e->arrow.xa, e->arrow.ya)) {
        //only drop arrows shot by player
        if(e->arrow.parent->type == ENTITY_PLAYER) addItemsToWorld(newItem(e->arrow.itemID,1), e->level, e->x+4, e->y+4, 1);
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
}
