#include "RenderLights.h"

#include "Batching.h"

//software rendering is slow -> bake light to texture
static void bakeLight(sf2d_texture* texture, int x, int y, int r) {
    int x0 = x - r;
    int x1 = x + r;
    int y0 = y - r;
    int y1 = y + r;

    // The game's resolution is actually 200x120, the textures are all scaled up by 2.
    if (x0 < 0)
        x0 = 0;
    if (y0 < 0)
        y0 = 0;
    if (x1 > texture->width)
        x1 = texture->width;
    if (y1 > texture->height)
        y1 = texture->height;

    int xx, yy;
    for (yy = y0; yy < y1; yy++) {
        int yd = yy - y;
        yd = yd * yd;
        for (xx = x0; xx < x1; xx++) {
            int xd = xx - x;
            int dist = xd * xd + yd;
            if (dist <= r * r) {
                if (dist >= (r - (r / 8)) * (r - (r / 8))) {
                    if (xd % 2 == 0 && yd % 2 == 0) {
                        sf2d_set_pixel(texture, xx, yy, 0xFF000000); // set transparent pixel
                    }
                } else if (dist >= (r - (r / 4)) * (r - (r / 4))) {
                    if ((xd % 2 == 0 && yd % 2 != 0) || (xd % 2 != 0 && yd % 2 == 0)) {
                        sf2d_set_pixel(texture, xx, yy, 0xFF000000); // set transparent pixel
                    }
                } else {
                    sf2d_set_pixel(texture, xx, yy, 0xFF000000); // set transparent pixel
                }
            }
        }
    }

    sf2d_texture_tile32(texture);
}

static bool inBatch = false;

void bakeLights() {
    playerLightBake = sf2d_create_texture(64, 64, TEXFMT_RGBA8, SF2D_PLACE_RAM);
    lanternLightBake = sf2d_create_texture(128, 128, TEXFMT_RGBA8, SF2D_PLACE_RAM);
    
    glowwormLightBake = sf2d_create_texture(32, 32, TEXFMT_RGBA8, SF2D_PLACE_RAM);
    glowwormBigLightBake = sf2d_create_texture(64, 64, TEXFMT_RGBA8, SF2D_PLACE_RAM);

    bakeLight(playerLightBake, 32, 32, 32);
    bakeLight(lanternLightBake, 64, 64, 64);
    
    bakeLight(glowwormLightBake, 8, 8, 8);
    bakeLight(glowwormBigLightBake, 12, 12, 12);
}

void freeLightBakes() {
    sf2d_free_texture(playerLightBake);
    sf2d_free_texture(lanternLightBake);
    
    sf2d_free_texture(glowwormLightBake);
    sf2d_free_texture(glowwormBigLightBake);
}

void renderLightsToStencil(PlayerData *pd, bool force, bool invert, bool rplayer) {
    if (force || (pd->entity.level > 1 && pd->entity.level != 5)) {
        C3D_DepthTest(true, GPU_NEVER, 0);
        C3D_StencilTest(true, GPU_NEVER, 1, 0xFF, 0xFF);
        C3D_StencilOp(GPU_STENCIL_REPLACE, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
        C3D_AlphaTest(true, GPU_GREATER, 0);
        
        batch_start();
        inBatch = true;

        if(pd->activeItem->id == ITEM_LANTERN) renderLight(pd->entity.x, pd->entity.y, lanternLightBake);
        else if(rplayer) renderLight(pd->entity.x, pd->entity.y, playerLightBake);
        
        int i;
        for (i = 0; i < eManager.lastSlot[pd->entity.level]; ++i) {
            Entity e = eManager.entities[pd->entity.level][i];
            if (e.type == ENTITY_FURNITURE) {
                if (e.entityFurniture.itemID == ITEM_LANTERN && e.x > pd->entity.x - 160 && e.y > pd->entity.y - 125 && e.x < pd->entity.x + 160 && e.y < pd->entity.y + 125)
                    renderLight(e.x, e.y, lanternLightBake);
            } else if(e.type == ENTITY_GLOWWORM && e.x > pd->entity.x - 160 && e.y > pd->entity.y - 125 && e.x < pd->entity.x + 160 && e.y < pd->entity.y + 125) { //TODO could be made smaller becuase of smaller light radius
                if(rand()%10==0) continue;
                else if(rand()%100==0) renderLight(e.x+20, e.y-20, glowwormBigLightBake);
                else renderLight(e.x+8, e.y-8, glowwormLightBake);
            }
        }
        
        int xo = offsetX >> 4;
        int yo = offsetY >> 4;
        int x, y;
        //added offset to render lights from lava which is offscreen
        //(combined with batching this should now finally run smoothly even on o3DS)
        for (x = xo-2; x <= 13 + xo+2; ++x) {
            for (y = yo-2; y <= 8 + yo+2; ++y) {
                if(getTile(pd->entity.level, x, y) == TILE_LAVA) {
                    //experimental "speedhack"
                    if(getTile(pd->entity.level, x+1,y)==TILE_LAVA && getTile(pd->entity.level, x-1,y)==TILE_LAVA && getTile(pd->entity.level, x,y+1)==TILE_LAVA && getTile(pd->entity.level, x,y-1)==TILE_LAVA) {
                        if((x+y)%2 == 0) continue;
                    }
                    renderLight((x << 4) + 8, (y << 4) + 8, playerLightBake);
                }
            }
        }
        
        inBatch = false;
        batch_end();

        C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
        if(invert) {
            C3D_StencilTest(true, GPU_EQUAL, 0, 0xFF, 0x0);
        } else {
            C3D_StencilTest(true, GPU_EQUAL, 1, 0xFF, 0x0);
        }
        C3D_AlphaTest(false, GPU_ALWAYS, 0x00);
        C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_REPLACE);
    }
}

void renderLight(int x, int y, sf2d_texture* texture) {
    if(inBatch) {
        batch_texture_scale(texture, (x - (texture->width / 2) - offsetX) * 2,
            (y - (texture->height / 2) - offsetY) * 2, 2.f, 2.f);
    } else {
        sf2d_draw_texture_scale(texture, (x - (texture->width / 2) - offsetX) * 2,
            (y - (texture->height / 2) - offsetY) * 2, 2.f, 2.f);
    }
}

void resetStencil() {
    C3D_StencilTest(false, GPU_ALWAYS, 0x00, 0xFF, 0x00);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}
