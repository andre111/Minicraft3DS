#include "../Entity.h"

#include "../Data.h"

Entity newEntityNPC(int type, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_NPC;
    e.level = level;
    e.x = x;
    e.y = y;
    e.hurtTime = 0;
    e.xKnockback = 0;
    e.yKnockback = 0;
    e.npc.type = type;
    e.xr = 4;
    e.yr = 3;
    e.canPass = false;
    
    e.tickFunction = NULL;
    
    return e;
}
