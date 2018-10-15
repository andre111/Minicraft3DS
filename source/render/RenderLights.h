#pragma once

#include "../Render.h"
#include "../Globals.h"

sf2d_texture *playerLightBake;
sf2d_texture *lanternLightBake;
sf2d_texture *glowwormLightBake;
sf2d_texture *glowwormBigLightBake;

void bakeLights();
void freeLightBakes();

void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer);
void renderLight(int x, int y, sf2d_texture* texture);
void resetStencil();