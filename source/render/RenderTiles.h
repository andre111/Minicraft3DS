#pragma once

#include <3ds.h>

void renderTile(int i, int d, u8 level, int x, int y);
void renderConnectedTile4(int x, int y, u32 xTile, u32 yTile);
void renderConnectedTile8(int x, int y, u32 xTile, u32 yTile);
void renderBackground(s8 level, int xScroll, int yScroll);
