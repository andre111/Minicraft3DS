#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityGlowworm(Entity *e, PlayerData *nearestPlayer);

Entity newEntityGlowworm(int x, int y, int level) {
    Entity e;
    e.type = ENTITY_GLOWWORM;
    e.level = level;
    e.glowworm.xa = 0;
    e.glowworm.ya = 0;
    e.glowworm.randWalkTime = 0;
    e.glowworm.waitTime = 0;
    e.x = x;
    e.y = y;
    e.canPass = true;
    
    e.tickFunction = &tickEntityGlowworm;
    
    return e;
}

void tickEntityGlowworm(Entity *e, PlayerData *nearestPlayer) {
    if(worldData.daytime>5000 && worldData.daytime<6000) {
        if(rand()%200==0) {
            removeEntityFromList(e, e->level, &eManager);
            return;
        }
    } else if(worldData.daytime>6000 && worldData.daytime<18000) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    
    int gspeed = (((syncTickCount & 0x3) == 3) ? 1 : 0);
    if (!moveMob(e, e->glowworm.xa * gspeed, e->glowworm.ya * gspeed) || (e->glowworm.randWalkTime==0) || (rand()%20) == 0) {
        if(e->glowworm.randWalkTime != 0) {
            e->glowworm.waitTime = 20 + (rand()%60);
        }
        if(e->glowworm.waitTime == 0 || getTile(e->level, (e->x)>>4, (e->y)>>4)!=TILE_TREE) {
            e->glowworm.randWalkTime = 20;
            e->glowworm.xa = ((rand()%3) - 1) * (rand()%2);
            e->glowworm.ya = ((rand()%3) - 1) * (rand()%2);
        } else {
            e->glowworm.xa = 0;
            e->glowworm.ya = 0;
        }
    }
    if (e->glowworm.randWalkTime > 0) {
        e->glowworm.randWalkTime--;
        if(e->glowworm.randWalkTime==0 && (e->glowworm.xa != 0 || e->glowworm.xa != 0)) {
            e->glowworm.waitTime = 120 + (rand()%60);
        }
    } else if (e->glowworm.waitTime > 0) {
        e->glowworm.waitTime--;
    }
}
