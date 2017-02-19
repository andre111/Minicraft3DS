#pragma once
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <3ds.h>

#include "Globals.h"

float nextFloat();
double sample(double * values, int x, int y);
double * Noise(int w, int h, int featureSize);
void newSeed();
void createAndValidateTopMap(int w, int h, int level, u8 * map, u8 * data);
void createTopMap(int w, int h, int level, u8 * map, u8 * data);
void createAndValidateUndergroundMap(int w, int h, int depthLevel, int level, u8 * map, u8 * data);
void createUndergroundMap(int w, int h, int depthLevel, int level, u8 * map, u8 * data);
void createAndValidateDungeonMap(int w, int h, int level, u8 * map, u8 * data);
void createDungeonMap(int w, int h, int level, u8 * map, u8 * data);
void createAndValidateSkyMap(int w, int h, int level, u8 * map, u8 * data);
void createSkyMap(int w, int h, int level, u8 * map, u8 * data);

int featureX;
int featureY;
void findFeatureLocation(int fw, int fh, int * accepted, int aLength, int maxTries, int w, int h, u8 * map, u8 * data);

void createVillage(int w, int h, int level, u8 * map, u8 * data);
void createDwarfHouse(int w, int h, int level, u8 * map, u8 * data);

bool hasNPC;
void createDungeonRoom(int w, int h, bool dragon, int level, u8 * map, u8 * data);