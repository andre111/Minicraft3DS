#include "../Entity.h"

#include "../Data.h"

void tickParticleSmash(Entity *e, PlayerData *nearestPlayer);

Entity newParticleSmash(int x, int y, int level) {
    Entity e;
    e.type = ENTITY_SMASHPARTICLE;
    e.level = level;
    e.smashParticle.age = 0;
    e.x = x;
    e.y = y;
    e.canPass = true;
    playSoundPositioned(snd_monsterHurt, e.level, e.x, e.y); //TODO: This is a wierd location for the effect
    
    e.tickFunction = &tickParticleSmash;
    
    return e;
}

void tickParticleSmash(Entity *e, PlayerData *nearestPlayer) {
    ++e->smashParticle.age;
    if(e->smashParticle.age > 10) {
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
}
