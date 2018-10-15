#pragma once

#define EFFECT_UNDYING 0
#define EFFECT_REGENERATION 1
#define EFFECT_SPEED 2
#define EFFECT_STRENGTH 3
#define EFFECT_SHIELDING 4
#define EFFECT_NIGHTVISION 5

#define EFFECTS_MAX 6
#define EFFECTS_DURATION_INFINITE 4294967295

void effectsDataInit();

char* effectGetName(int id);
int effectGetIconX(int id);
int effectGetIconY(int id);
