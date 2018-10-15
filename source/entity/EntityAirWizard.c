#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityAirWizard(Entity *e, PlayerData *nearestPlayer);

Entity newEntityAirWizard(int x, int y, int level) {
    Entity e;
    e.type = ENTITY_AIRWIZARD;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.wizard.dir = 0;
    e.wizard.health = 2000;
    e.wizard.attackDelay = 0;
    e.wizard.attackTime = 0;
    e.wizard.attackType = 0;
    e.wizard.xa = 0;
    e.wizard.ya = 0;
    e.xr = 4;
    e.yr = 3;
    e.canPass = true;
    
    e.tickFunction = &tickEntityAirWizard;
    
    return e;
}

void tickEntityAirWizard(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0) e->hurtTime--;
    
    if (e->wizard.attackDelay > 0) {
        e->wizard.dir = (e->wizard.attackDelay - 45) / 4 % 4;
        e->wizard.dir = (e->wizard.dir * 2 % 4) + (e->wizard.dir / 2);
        if (e->wizard.attackDelay < 45) e->wizard.dir = 0;
        e->wizard.attackDelay--;
        if (e->wizard.attackDelay <= 0) {
            e->wizard.attackType = 0;
            if (e->wizard.health < 1000) e->wizard.attackType = 1;
            if (e->wizard.health < 200) e->wizard.attackType = 2;
            e->wizard.attackTime = 120;
        }
        return;
    }
    
    if (e->wizard.attackTime > 0) {
        e->wizard.attackTime--;
        float dir = e->wizard.attackTime * 0.25 * (e->wizard.attackTime % 2 * 2 - 1);
        float speed = (0.7) + (e->wizard.attackType+1) * 0.2;
        addEntityToList(newEntitySpark(e, cos(dir) * speed, sin(dir) * speed), &eManager);
        return;
    }
    
    if (e->wizard.randWalkTime == 0 && nearestPlayer!=NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        int dist = xd * xd + yd * yd;
        if (dist > 80 * 80) {
            e->wizard.xa = 0;
            e->wizard.ya = 0;
            if (xd < 0) e->wizard.xa = -1; 
            if (xd > 0) e->wizard.xa = +1;
            if (yd < 0) e->wizard.ya = -1;
            if (yd > 0) e->wizard.ya = +1;
        } else if (dist < 24 * 24) {
            e->wizard.xa = 0;
            e->wizard.ya = 0;
            if (xd < 0) e->wizard.xa = +1; 
            if (xd > 0) e->wizard.xa = -1;
            if (yd < 0) e->wizard.ya = +1;
            if (yd > 0) e->wizard.ya = -1;
        }
    }
    
    int wSpeed = (syncTickCount % 4) == 0 ? 0 : 1;
    if (!moveMob(e, e->wizard.xa * wSpeed, e->wizard.ya * wSpeed) || (rand()%100) == 0) {
        e->wizard.randWalkTime = 30;
        e->wizard.xa = ((rand()%3) - 1) * (rand()%2);
        e->wizard.ya = ((rand()%3) - 1) * (rand()%2);
    }
    
    if(e->wizard.xa != 0 || e->wizard.ya != 0){ 
        e->wizard.walkDist++;
        awX = e->x;
        awY = e->y;
    }
    
    if(e->wizard.xa < 0) e->wizard.dir = 2;
    else if(e->wizard.xa > 0) e->wizard.dir = 3;
    if(e->wizard.ya < 0) e->wizard.dir = 1;
    else if(e->wizard.ya > 0) e->wizard.dir = 0;
    
    if (e->wizard.randWalkTime > 0) {
        e->wizard.randWalkTime--;
        if (e->wizard.randWalkTime == 0 && nearestPlayer!=NULL) {
            int xd = nearestPlayer->entity.x - e->x;
            int yd = nearestPlayer->entity.y - e->y;
            if (rand()%4 == 0 && xd * xd + yd * yd < 50 * 50) {
                if (e->wizard.attackDelay == 0 && e->wizard.attackTime == 0) e->wizard.attackDelay = 120;
            }
        }
    }
}
