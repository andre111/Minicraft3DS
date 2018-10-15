#pragma once

#include <3ds.h>

void renderText(char * msg, u32 x, u32 y);
void renderTextSized(char * msg, u32 x, u32 y, float size);
void renderTextColor(char * msg, u32 x, u32 y, u32 color);
void renderTextColorSized(char * msg, int x, int y, float size, u32 color);
void renderTextColorSpecial(char * msg, u32 x, u32 y, u32 color, u32 color2);
