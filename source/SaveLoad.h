#pragma once
#include <stdio.h>
#include <string.h>
#include "Entity.h"
#include "Globals.h"

void saveCurrentWorld(char * filename, EntityManager * eManager, Entity * player,  u8 * map, u8 * mapData);
int loadWorld(char * filename, EntityManager * eManager, Entity * player, u8 * map, u8 * mapData);
