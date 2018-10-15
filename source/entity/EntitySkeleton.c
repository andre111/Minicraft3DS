#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntitySkeleton(Entity *e, PlayerData *nearestPlayer);

Entity newEntitySkeleton(int lvl, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_SKELETON;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.hostile.lvl = lvl;
    e.hostile.xa = 0;
    e.hostile.ya = 0;
    e.hostile.health = lvl * lvl * 10;
    e.hostile.dir = 0;
    e.hostile.randAttackTime = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    switch(lvl) {
        case 2: e.hostile.color = 0xFFC4C4C4; break;
        case 3: e.hostile.color = 0xFFA0A0A0; break;
        case 4: e.hostile.color = 0xFF7A7A7A; break;
        default: e.hostile.color = 0xFFFFFFFF; break;
    }
    
    e.tickFunction = &tickEntitySkeleton;
    
    return e;
}

void tickEntitySkeleton(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0) e->hurtTime--;
    
    if(e->hostile.xa < 0) e->hostile.dir = 2;
    else if(e->hostile.xa > 0) e->hostile.dir = 3;
    if(e->hostile.ya < 0) e->hostile.dir = 1;
    else if(e->hostile.ya > 0) e->hostile.dir = 0;
    
    //shooting arrows
    --(e->hostile.randAttackTime);
    if(e->hostile.randAttackTime <= 0) {
        e->hostile.randAttackTime = 80 - (e->hostile.lvl * 5);
        
        int aitemID = ITEM_ARROW_WOOD;
        if(e->hostile.lvl >= 2) aitemID = ITEM_ARROW_STONE;
        
        //turn to player when attacking
        if(nearestPlayer!=NULL) {
            int xd = nearestPlayer->entity.x - e->x;
            int yd = nearestPlayer->entity.y - e->y;
            if(xd*xd > yd*yd) {
                if (xd < 0) e->hostile.dir = 2;
                if (xd > 0) e->hostile.dir = 3;
            } else {
                if (yd < 0) e->hostile.dir = 1;
                if (yd > 0) e->hostile.dir = 0;
            }
        }
        
        switch(e->hostile.dir) {
        case 0:
            addEntityToList(newEntityArrow(e, aitemID, 0, 2, e->level), &eManager);
            break;
        case 1:
            addEntityToList(newEntityArrow(e, aitemID, 0, -2, e->level), &eManager);
            break;
        case 2:
            addEntityToList(newEntityArrow(e, aitemID, -2, 0, e->level), &eManager);
            break;
        case 3:
            addEntityToList(newEntityArrow(e, aitemID, 2, 0, e->level), &eManager);
            break;
        }
    }
    
    
    if(e->hostile.xa != 0 || e->hostile.ya != 0) e->hostile.walkDist++;

    int speed = syncTickCount & 1;
    if (!moveMob(e, e->hostile.xa * speed, e->hostile.ya * speed) || (rand()%100) == 0) {
        e->hostile.randWalkTime = 60;
        e->hostile.xa = ((rand()%3) - 1) * (rand()%2);
        e->hostile.ya = ((rand()%3) - 1) * (rand()%2);
    }
    if (e->hostile.randWalkTime > 0) e->hostile.randWalkTime--;
}
