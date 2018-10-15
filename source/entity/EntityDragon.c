#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntityDragon(Entity *e, PlayerData *nearestPlayer);

Entity newEntityDragon(int x, int y, int level) {
    Entity e;
    e.type = ENTITY_DRAGON;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.dragon.dir = 0;
    e.dragon.health = 2000;
    e.dragon.attackDelay = 0;
    e.dragon.attackTime = 0;
    e.dragon.attackType = 0;
    e.dragon.animTimer = 0;
    e.dragon.xa = 0;
    e.dragon.ya = 0;
    e.xr = 8;
    e.yr = 8;
    e.canPass = false;
    
    e.tickFunction = &tickEntityDragon;
    
    return e;
}

void tickEntityDragon(Entity *e, PlayerData *nearestPlayer) {
    if (e->hurtTime > 0) e->hurtTime--;
    
    e->dragon.animTimer++;
    if(e->dragon.animTimer>=4*4) {
        e->dragon.animTimer = 0;
    }
    
    //choose random attack
    if (e->dragon.attackDelay > 0) {
        e->dragon.attackDelay--;
        if (e->dragon.attackDelay <= 0) {
            e->dragon.attackType = rand()%2;
            e->dragon.attackTime = 121;
        }
        return;
    }
    
    if (e->dragon.attackTime > 0) {
        e->dragon.attackTime--;
        
        //turn to player when attacking
        if(nearestPlayer!=NULL) {
            int xd = nearestPlayer->entity.x - e->x;
            int yd = nearestPlayer->entity.y - e->y;
            if(xd*xd > yd*yd) {
                if (xd < 0) e->dragon.dir = 2;
                if (xd > 0) e->dragon.dir = 3;
            } else {
                if (yd < 0) e->dragon.dir = 1;
                if (yd > 0) e->dragon.dir = 0;
            }
        }
        
        switch(e->dragon.attackType) {
        case 0: //Firebreathing
            if(e->dragon.attackTime%2 == 0) {
                float dfdir = 0;
                
                if(e->dragon.dir==0) dfdir = 1 * 3.141592 / 2;
                else if(e->dragon.dir==1) dfdir = 3 * 3.141592 / 2;
                else if(e->dragon.dir==2) dfdir = 2 * 3.141592 / 2;
                else if(e->dragon.dir==3) dfdir = 0 * 3.141592 / 2;
                
                dfdir += 0.03141592 * ((rand()%33) - 16);
                
                addEntityToList(newEntityDragonFire(e, e->dragon.attackType, e->x + cos(dfdir)*14, e->y + sin(dfdir)*14, cos(dfdir), sin(dfdir)), &eManager);
            }
        break;
        case 1: //Firering
            if(e->dragon.attackTime%20 == 0) {
                int ai = 0;
                for(ai = 0; ai < 16; ai++) {
                    float ddir = (3.141592 * 2 / 16.0) * ai;
                    float ddist = (140 - e->dragon.attackTime) / 2;
                    
                    addEntityToList(newEntityDragonFire(e, e->dragon.attackType, (e->x) + cos(ddir)*ddist, (e->y) + sin(ddir)*ddist, 0, 0), &eManager);
                }
            }
        break;
        }
        
        return;
    }
    
    //TODO - movement copied from airwizard, adjust to better fit dragon
    if (e->dragon.randWalkTime == 0 && nearestPlayer!=NULL) {
        int xd = nearestPlayer->entity.x - e->x;
        int yd = nearestPlayer->entity.y - e->y;
        int dist = xd * xd + yd * yd;
        if (dist > 64 * 64) {
            e->dragon.xa = 0;
            e->dragon.ya = 0;
            if (xd < 0) e->dragon.xa = -1; 
            if (xd > 0) e->dragon.xa = +1;
            if (yd < 0) e->dragon.ya = -1;
            if (yd > 0) e->dragon.ya = +1;
        } else if (dist < 16 * 16) {
            e->dragon.xa = 0;
            e->dragon.ya = 0;
            if (xd < 0) e->dragon.xa = +1; 
            if (xd > 0) e->dragon.xa = -1;
            if (yd < 0) e->dragon.ya = +1;
            if (yd > 0) e->dragon.ya = -1;
        }
    }
    
    int dSpeed = (syncTickCount % 4) == 0 ? 0 : 1;
    if (!moveMob(e, e->dragon.xa * dSpeed, e->dragon.ya * dSpeed) || (rand()%120) == 0) {
        e->dragon.randWalkTime = 30;
        e->dragon.xa = ((rand()%3) - 1) * (rand()%2);
        e->dragon.ya = ((rand()%3) - 1) * (rand()%2);
    }
    
    if(e->dragon.xa != 0 || e->dragon.ya != 0){ 
        e->dragon.walkDist++;
    }
    
    if(e->dragon.xa < 0) e->dragon.dir = 2;
    else if(e->dragon.xa > 0) e->dragon.dir = 3;
    if(e->dragon.ya < 0) e->dragon.dir = 1;
    else if(e->dragon.ya > 0) e->dragon.dir = 0;
    
    //if (e->dragon.randWalkTime > 0) {
    //    e->dragon.randWalkTime--;
    //    if (e->dragon.randWalkTime == 0) {
        if(nearestPlayer!=NULL) {
            int xd = nearestPlayer->entity.x - e->x;
            int yd = nearestPlayer->entity.y - e->y;
            if (rand()%12 == 0 && xd * xd + yd * yd < 50 * 50) {
                if (e->dragon.attackDelay == 0 && e->dragon.attackTime == 0) e->dragon.attackDelay = 40;
            }
        }
    //    }
    //}
}
