#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityPassive(Entity *e, PlayerData *nearestPlayer);

Entity newEntityPassive(int type, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_PASSIVE;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.passive.mtype = type;
    e.passive.health = 20;
    e.passive.dir = 0;
    e.passive.xa = 0;
    e.passive.ya = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    
    e.tickFunction = &tickEntityPassive;
    
    return e;
}

void tickEntityPassive(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0) e->hurtTime--;
    if (e->passive.randWalkTime == 0 && nearestPlayer!=NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        //flee from player
        if (xd * xd + yd * yd < 40 * 40) {
            e->passive.xa = 0;
            e->passive.ya = 0;
            if (xd < 0) e->passive.xa = +1; 
            if (xd > 0) e->passive.xa = -1;
            if (yd < 0) e->passive.ya = +1;
            if (yd > 0) e->passive.ya = -1;
        }
    }
    
    if(e->passive.xa < 0) e->passive.dir = 2;
    else if(e->passive.xa > 0) e->passive.dir = 3;
    if(e->passive.ya < 0) e->passive.dir = 1;
    else if(e->passive.ya > 0) e->passive.dir = 0;
    
    if(e->passive.xa != 0 || e->passive.ya != 0) e->passive.walkDist++;

    int pspeed = syncTickCount & 1;
    if (!moveMob(e, e->passive.xa * pspeed, e->passive.ya * pspeed) || (rand()%100) == 0) {
        e->passive.randWalkTime = 60;
        e->passive.xa = ((rand()%3) - 1) * (rand()%2);
        e->passive.ya = ((rand()%3) - 1) * (rand()%2);
    }
    if (e->passive.randWalkTime > 0) e->passive.randWalkTime--;
}
