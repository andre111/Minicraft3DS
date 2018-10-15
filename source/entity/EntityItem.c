#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"
#include "../network/Synchronizer.h"

void tickEntityItem(Entity *e, PlayerData *nearestPlayer);

Entity newEntityItem(Item item, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_ITEM;
    e.level = level;
    e.entityItem.age = 0;
    e.entityItem.item = item;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = false;
    
    e.tickFunction = &tickEntityItem;
    
    e.entityItem.xx = x;
    e.entityItem.yy = y;
    e.entityItem.zz = 2;
    e.entityItem.xa = gaussrand(false) * 0.1;
    e.entityItem.ya = gaussrand(false) * 0.1;
    e.entityItem.za = ((float)rand() / RAND_MAX) * 0.45 + 1;
    
    return e;
}

void tickEntityItem(Entity *e, PlayerData *nearestPlayer) {
    ++e->entityItem.age;
    if(e->entityItem.age == 630){ 
        removeEntityFromList(e, e->level, &eManager);
        /*
        Programming pro tip: 
            Remember to put a return statement after you remove the entity,
            or else your going to have a very bad time like I did.
        */
        return;
    }
    e->entityItem.xx += e->entityItem.xa;
    e->entityItem.yy += e->entityItem.ya;
    e->entityItem.zz += e->entityItem.za;
    if (e->entityItem.zz < 0) {
        e->entityItem.zz = 0;
        e->entityItem.za *= -0.5;
        e->entityItem.xa *= 0.6;
        e->entityItem.ya *= 0.6;
    }
    e->entityItem.za -= 0.15;
    int ox = e->x;
    int oy = e->y;
    int nx = (int) e->entityItem.xx;
    int ny = (int) e->entityItem.yy;
    int expectedx = nx - e->x;
    int expectedy = ny - e->y;
    move(e, nx - e->x, ny - e->y);
    int gotx = e->x - ox;
    int goty = e->y - oy;
    e->entityItem.xx += gotx - expectedx;
    e->entityItem.yy += goty - expectedy;
}
