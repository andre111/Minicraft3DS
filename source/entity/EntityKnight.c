#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityKnight(Entity *e, PlayerData *nearestPlayer);

Entity newEntityKnight(int lvl, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_KNIGHT;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.hostile.lvl = lvl;
    e.hostile.xa = 0;
    e.hostile.ya = 0;
    e.hostile.health = lvl * lvl * 20;
    e.hostile.dir = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    switch(lvl) {
        case 2: e.hostile.color = 0xFF0000C6; break;
        case 3: e.hostile.color = 0xFF00A3C6; break;
        case 4: e.hostile.color = 0xFF707070; break;
        default: e.hostile.color = 0xFFFFFFFF; break;
    }
    
    e.tickFunction = &tickEntityKnight;
    
    return e;
}

void tickEntityKnight(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0) e->hurtTime--;
    if (e->hostile.randWalkTime == 0 && e->type != ENTITY_SKELETON && nearestPlayer!=NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        int dist = 80 * 80;
        
        //charge player
        if (xd * xd + yd * yd < dist) {
            e->hostile.xa = 0;
            e->hostile.ya = 0;
            if (xd < 0) e->hostile.xa = -1; 
            if (xd > 0) e->hostile.xa = +1;
            if (yd < 0) e->hostile.ya = -1;
            if (yd > 0) e->hostile.ya = +1;
        }
    }
    
    if(e->hostile.xa < 0) e->hostile.dir = 2;
    else if(e->hostile.xa > 0) e->hostile.dir = 3;
    if(e->hostile.ya < 0) e->hostile.dir = 1;
    else if(e->hostile.ya > 0) e->hostile.dir = 0;
    
    if(e->hostile.xa != 0 || e->hostile.ya != 0) e->hostile.walkDist++;

    int speed = syncTickCount & 1;
    if (!moveMob(e, e->hostile.xa * speed, e->hostile.ya * speed) || (rand()%100) == 0) {
        e->hostile.randWalkTime = 60;
        e->hostile.xa = ((rand()%3) - 1) * (rand()%2);
        e->hostile.ya = ((rand()%3) - 1) * (rand()%2);
    }
    if (e->hostile.randWalkTime > 0) e->hostile.randWalkTime--;
}
