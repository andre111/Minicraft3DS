#pragma once

#define LOAD_TO_SINGLEPLAYER 1
#define LOAD_TO_MULTIPLAYER 2
#define LOAD_TO_EDITOR 3

void menuLoadGameInit(int target);
void menuLoadGameTick();
void menuLoadGameRender();
