#include "../Entity.h"

#include "../Data.h"

Entity newEntityMagicPillar(int x, int y, int level) {
    Entity e;
    e.type = ENTITY_MAGIC_PILLAR;
    e.level = level;
    e.x = x;
    e.y = y;
    e.xr = 3;
    e.yr = 3;
    e.canPass = false;
    
    e.tickFunction = NULL;
    
    return e;
}
