#include "../Entity.h"

#include "../Data.h"
//TODO: remove this roundabout include
#include "../network/Synchronizer.h"

void tickParticleText(Entity *e, PlayerData *nearestPlayer);

Entity newParticleText(char * str, u32 color, int x, int y, int level) {
    Entity e;
    e.type = ENTITY_TEXTPARTICLE;
    e.level = level;
    e.textParticle.color = color;
    e.textParticle.age = 0;
    e.textParticle.text = (char*)calloc(strlen(str),sizeof(char)); 
    strncpy(e.textParticle.text,str,strlen(str)); 
    e.x = x;
    e.y = y;
    e.canPass = true;
    e.textParticle.xx = x;
    e.textParticle.yy = y;
    e.textParticle.zz = 2;
    e.textParticle.xa = gaussrand(false) * 0.3;
    e.textParticle.ya = gaussrand(false) * 0.2;
    e.textParticle.za = ((float)rand() / RAND_MAX) * 0.7 + 2;
    
    e.tickFunction = &tickParticleText;
    
    return e;
}

void tickParticleText(Entity *e, PlayerData *nearestPlayer) {
    ++e->textParticle.age;
    if(e->textParticle.age == 60){ 
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->textParticle.xx += e->textParticle.xa;
    e->textParticle.yy += e->textParticle.ya;
    e->textParticle.zz += e->textParticle.za;
    if (e->textParticle.zz < 0) {
        e->textParticle.zz = 0;
        e->textParticle.za *= -0.5;
        e->textParticle.xa *= 0.6;
        e->textParticle.ya *= 0.6;
    }
    e->textParticle.za -= 0.15;
    e->x = (int) e->textParticle.xx;
    e->y = (int) e->textParticle.yy;
}
