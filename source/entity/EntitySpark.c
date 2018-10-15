#include "../Entity.h"

#include "../Data.h"
#include "../Globals.h"

void tickEntitySpark(Entity *e, PlayerData *nearestPlayer);

Entity newEntitySpark(Entity* parent, float xa, float ya) {
    Entity e;
    e.type = ENTITY_SPARK;
    e.level = parent->level;
    e.spark.age = 0;
    e.spark.parent = parent;
    e.spark.xa = xa;
    e.spark.ya = ya;
    e.spark.xx = parent->x;
    e.spark.yy = parent->y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = true;
    
    e.tickFunction = &tickEntitySpark;
    
    return e;
}

void tickEntitySpark(Entity *e, PlayerData *nearestPlayer) {
    e->spark.age++;
    if (e->spark.age >= 260) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->spark.xx += e->spark.xa;
    e->spark.yy += e->spark.ya;
    e->x = (int) e->spark.xx;
    e->y = (int) e->spark.yy;
    
    if(nearestPlayer!=NULL && intersects(nearestPlayer->entity, e->x + e->spark.xa - e->xr, e->y + e->spark.ya - e->yr, e->x + e->spark.xa + e->xr, e->y + e->spark.ya + e->yr)){
        EntityVsEntity(e, &(nearestPlayer->entity));
        removeEntityFromList(e, e->level, &eManager);
    }
}
