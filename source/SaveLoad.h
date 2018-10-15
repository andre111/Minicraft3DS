#pragma once

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include "Entity.h"
#include "Player.h"
#include "Globals.h"

#define SAVE_VERSION 2

#define SAVE_COMMENT "Minicraft3DSSave"
#define SAVE_COPYBUFFER_SIZE 4096

bool saveFileCopy(char *target, char *source);

bool saveWorld(char *filename,  EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);
bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount);

s8 checkFileNameForErrors(char *filename);
