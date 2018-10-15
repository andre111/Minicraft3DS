#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityDragonFire(Entity *e, PlayerData *nearestPlayer);

Entity newEntityDragonFire(Entity* parent, u8 type, int x, int y, float xa, float ya) {
    Entity e;
    e.type = ENTITY_DRAGONPROJECTILE;
    e.level = parent->level;
    e.dragonFire.age = 0;
    e.dragonFire.type = type;
    e.dragonFire.parent = parent;
    e.dragonFire.xa = xa;
    e.dragonFire.ya = ya;
    e.dragonFire.xx = x;
    e.dragonFire.yy = y;
    e.x = (int) x;
    e.y = (int) y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = true;
    
    e.tickFunction = &tickEntityDragonFire;
    
    return e;
}

void tickEntityDragonFire(Entity *e, PlayerData *nearestPlayer) {
    e->dragonFire.age++;
    if (e->dragonFire.age >= 30) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->dragonFire.xx += e->dragonFire.xa;
    e->dragonFire.yy += e->dragonFire.ya;
    e->x = (int) e->dragonFire.xx;
    e->y = (int) e->dragonFire.yy;
    
    if(nearestPlayer!=NULL && intersects(nearestPlayer->entity, e->x + e->dragonFire.xa - e->xr, e->y + e->dragonFire.ya - e->yr, e->x + e->dragonFire.xa + e->xr, e->y + e->dragonFire.ya + e->yr)){
        EntityVsEntity(e, &(nearestPlayer->entity));
        removeEntityFromList(e, e->level, &eManager);
    }
}
