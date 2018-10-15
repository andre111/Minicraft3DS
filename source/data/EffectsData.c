#include "EffectsData.h"

static char* _effectNames[EFFECTS_MAX+1];
static int _effectIconX[EFFECTS_MAX+1];
static int _effectIconY[EFFECTS_MAX+1];

static void _effectRegister(int id, char* name, int iconX, int iconY) {
    _effectNames[id] = name;
    _effectIconX[id] = iconX;
    _effectIconY[id] = iconY;
}

void effectsDataInit() {
    _effectRegister(EFFECT_UNDYING, "Undying", 0, 352);
    _effectRegister(EFFECT_REGENERATION, "Regeneration", 8, 352);
    _effectRegister(EFFECT_SPEED, "Speed", 16, 352);
    _effectRegister(EFFECT_STRENGTH, "Strength", 24, 352);
    _effectRegister(EFFECT_SHIELDING, "Shielding", 32, 352);
    _effectRegister(EFFECT_NIGHTVISION, "Night Vision", 40, 352);
}


char* effectGetName(int id) {
    return _effectNames[id];
}

int effectGetIconX(int id) {
    return _effectIconX[id];
}

int effectGetIconY(int id) {
    return _effectIconY[id];
}
