#include "Render.h"

#include "render/Batching.h"

extern u32 syncTickCount;

void render(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 8;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 8;
    }
    sf2d_draw_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, 8, 8,
            scaleX, scaleY);
}

void renderb(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 8;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 8;
    }
    sf2d_draw_texture_part_scale_blend(icons, xp << 1, yp << 1, xTile, yTile, 8,
            8, scaleX, scaleY, color);
}

void renderr(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float rotate) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 8;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 8;
    }
    sf2d_draw_texture_part_rotate_scale(icons, xp << 1, yp << 1, rotate, xTile,
            yTile, 8, 8, scaleX, scaleY);
}

void renderc(s32 xp, s32 yp, u32 xTile, u32 yTile, int sizeX, int sizeY,
        u8 bits) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += sizeX;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += sizeY;
    }
    sf2d_draw_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, sizeX,
            sizeY, scaleX, scaleY);
}

void render16(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 16;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 16;
    }
    sf2d_draw_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, 16, 16,
            scaleX, scaleY);
}

void render16c(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, float scaleX,float scaleY) {
    xp -= offsetX;
    yp -= offsetY;
    xp *= scaleX;
    yp *= scaleY;
    if ((bits & 1) > 0) {
        xp += 16 * scaleX;
        scaleX = -scaleX;
    }
    if ((bits & 2) > 0) {
        yp += 16 * scaleY;
        scaleY = -scaleY;
    }
    sf2d_draw_texture_part_scale(icons, xp, yp, xTile, yTile,16, 16, scaleX, scaleY);
}

void render16b(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits, u32 color) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 16;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 16;
    }
    sf2d_draw_texture_part_scale_blend(icons, xp << 1, yp << 1, xTile, yTile,
            16, 16, scaleX, scaleY, color);
}

void render32(s32 xp, s32 yp, u32 xTile, u32 yTile, u8 bits) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = 2, scaleY = 2;
    if ((bits & 1) > 0) {
        scaleX = -2;
        xp += 32;
    }
    if ((bits & 2) > 0) {
        scaleY = -2;
        yp += 32;
    }
    sf2d_draw_texture_part_scale(icons, xp << 1, yp << 1, xTile, yTile, 32, 32,
            scaleX, scaleY);
}

int playerScale = 2;
void renderp(s32 xp, s32 yp, u32 xTile, u32 yTile) {
    xp -= offsetX;
    yp -= offsetY;
    int scaleX = playerScale, scaleY = playerScale;
    sf2d_draw_texture_part_scale(playerSprites, xp << 1, yp << 1, xTile, yTile, 16, 16, 
            scaleX, scaleY);
}

void renderTitle(int x, int y) {
    sf2d_draw_texture_part_scale(icons, (x - 26) << 1, y << 1, 0, 240, 104, 16, 2.0, 2.0); // MINICRAFT
    sf2d_draw_texture_part(icons, x + 48, y + 44, 104, 240, 152, 16); // 3DS HOMEBREW EDITION
}

void renderButtonIcon(u32 keyIcon, int x, int y, float scale) {
    switch (keyIcon) {
    case CIRCLEPAD:
        render16c(x, y, 96, 208, 0, scale, scale);
        break;
    case KEY_CPAD_UP:
        render16c(x, y, 112, 208, 0, scale, scale);
        break;
    case KEY_CPAD_LEFT:
        render16c(x, y, 128, 208, 0, scale, scale);
        break;
    case KEY_CPAD_DOWN:
        render16c(x, y, 144, 208, 0, scale, scale);
        break;
    case KEY_CPAD_RIGHT:
        render16c(x, y, 160, 208, 0, scale, scale);
        break;

        /* New 3DS only */
    case CSTICK:
        render16c(x, y, 176, 208, 0, scale, scale);
        break;
    case KEY_CSTICK_UP:
        render16c(x, y, 192, 208, 0, scale, scale);
        break;
    case KEY_CSTICK_LEFT:
        render16c(x, y, 208, 208, 0, scale, scale);
        break;
    case KEY_CSTICK_DOWN:
        render16c(x, y, 224, 208, 0, scale, scale);
        break;
    case KEY_CSTICK_RIGHT:
        render16c(x, y, 240, 208, 0, scale, scale);
        break;

    case KEY_A:
        render16c(x, y, 0, 224, 0, scale, scale);
        break;
    case KEY_B:
        render16c(x, y, 16, 224, 0, scale, scale);
        break;
    case KEY_X:
        render16c(x, y, 32, 224, 0, scale, scale);
        break;
    case KEY_Y:
        render16c(x, y, 48, 224, 0, scale, scale);
        break;
    case KEY_DUP:
        render16c(x, y, 64, 224, 0, scale, scale);
        break;
    case KEY_DLEFT:
        render16c(x, y, 80, 224, 0, scale, scale);
        break;
    case KEY_DDOWN:
        render16c(x, y, 96, 224, 0, scale, scale);
        break;
    case KEY_DRIGHT:
        render16c(x, y, 112, 224, 0, scale, scale);
        break;
    case KEY_START:
        render16c(x - 8, y, 128, 224, 0, scale, scale);
        render16c(x + 8, y, 144, 224, 0, scale, scale);
        break;
    case KEY_SELECT:
        render16c(x - 8, y, 160, 224, 0, scale, scale);
        render16c(x + 8, y, 176, 224, 0, scale, scale);
        break;
    case KEY_L:
        render16c(x, y, 192, 224, 0, scale, scale);
        break;
    case KEY_R:
        render16c(x, y, 208, 224, 0, scale, scale);
        break;

        /* New 3DS only */
    case KEY_ZL:
        render16c(x, y, 224, 224, 0, scale, scale);
        break;
    case KEY_ZR:
        render16c(x, y, 240, 224, 0, scale, scale);
        break;
    }
}

int getFrame(int a, int b, int s) {
    return (a == s) ? 0 : ((a < b - 1) ? 8 : 16);
}

void renderFrame(int x1, int y1, int x2, int y2, u32 bgColor) {
    int startX = x1;
    int startY = y1;
    sf2d_draw_rectangle((x1 << 4) + 4 - (offsetX << 1),
            (y1 << 4) + 4 - (offsetY << 1), ((x2 - x1) << 4) - 8,
            ((y2 - y1) << 4) - 8, bgColor);
            
    batch_start();
    while (x1 < x2) {
        y1 = startY;
        while (y1 < y2) {
            int xp = (x1 << 4) - (offsetX << 1);
            int yp = (y1 << 4) - (offsetY << 1);
            batch_texture_part_scale(icons, xp, yp,
                    getFrame(x1, x2, startX), 200 + getFrame(y1, y2, startY), 8,
                    8, 2.0, 2.0);
            ++y1;
        }
        ++x1;
    }
    batch_end();
}

void renderZoomedMap(PlayerData *pd) {
    sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
    int mx = pd->mapScrollX;
    int my = pd->mapScrollY;
    if(pd->mapZoomLevel == 2) mx = 32;
    sf2d_draw_texture_scale(minimap[pd->entity.level], mx, my, pd->mapZoomLevel, pd->mapZoomLevel); // zoomed map
    
    // Airwizard on zoomed map
    if(pd->entity.level == 0){
        if(awX != 0 && awY != 0){
            render16c(
            (mx+((awX/16)*pd->mapZoomLevel)-16)/2, 
            (my+((awY/16)*pd->mapZoomLevel)-16)/2, 
            160, 112, 
            ((pd->entity.p.walkDist >> 6) & 1) == 0 ? 0 : 1, 
            2, 2
            );
        }
    }
    // Player on zoomed map
    //TODO: Maybe also render other players?
    render16c(
    (mx+((pd->entity.x/16)*pd->mapZoomLevel)-16)/2, 
    (my+((pd->entity.y/16)*pd->mapZoomLevel)-16)/2, 
    0, 112, 
    ((pd->entity.p.walkDist >> 6) & 1) == 0 ? 0 : 1, 
    2, 2
    );
    
    renderText(pd->mapText,224, 214); // "x2"/"x4"/"x6"
    render16(142, 2, 72, 208, 0); // Exit button
    renderc(126, 102, 40, 208, 32, 16, 0); // Plus/Minus zoom buttons
    if(pd->mapZoomLevel < 3) sf2d_draw_rectangle(258, 210, 26, 20, 0x7F4F4F4F); // gray out minus button
    else if(pd->mapZoomLevel > 5) sf2d_draw_rectangle(284, 210, 26, 20, 0x7F4F4F4F); // gray out minus button
}

char scoreT[32];
void renderGui(PlayerData *pd) {
    int i;
    //health and stamina
    for (i = 0; i < 10; ++i) {
        if (i < pd->entity.p.health)
            render(i * 8 + 6, 5, 168, 152, 0);
        else
            render(i * 8 + 6, 5, 176, 152, 0);
        if (i < pd->entity.p.stamina)
            render(i * 8 + 6, 14, 184, 152, 0);
        else
            render(i * 8 + 6, 14, 191, 152, 0);
    }
    
    //minimap
    sf2d_draw_texture(minimap[pd->entity.level], 10, 102);
    
    //active item
    renderItemWithTextCentered(pd->activeItem, 320, 66);
    itoa(pd->score, scoreT, 10); // integer to base10 string
    renderText("Score:",214,12);
    renderText(scoreT,(140-(strlen(scoreT)*12))/2 + 180,29);
    if(pd->entity.level == 0){
        if(awX != 0 && awY != 0){
            renderc(1 + (awX/32), 47 + (awY/32), 88, 216, 8, 8, 0); // Mini-AWizard head.
        }
    }
    //TODO: Maybe also render other players?
    renderc(1 + (pd->entity.x/32), 47 + (pd->entity.y/32), 88, 208, 8, 8, 0); // Mini-Player head.
    
    //quick select
    renderText("Quickselect:",164,118);

    Inventory * inv = &(pd->inventory);
    Item * item;
    for (i = 0; i < 8; ++i) {
        if((inv->lastSlot) > i) {
            int xip = i % 4;
            int yip = i / 4;
        
            item = &inv->items[i];
            renderItemIcon(item->id, item->countLevel, 81+xip*21, 77+yip*21);
        }
    }
}

void renderPlayer(PlayerData *pd) {
    if (pd->entity.level!=getLocalPlayer()->entity.level) {
        return;
    }
    if (pd->entity.p.isDead) {
        return;
    }
    int xo = pd->entity.x - 8;
    int yo = pd->entity.y - 8;

    //attack animation upwards
    if (pd->entity.p.attackTimer > 0 && pd->entity.p.dir == 1) {
        renderc(xo, yo - 4, 16, 160, 16, 8, 0);
        renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 4, yo - 4);
    }
    
    //find basic indices
    int aIndexBig = 0;
    int aIndexSmall = 0;
    switch(pd->entity.p.dir) {
    case 0: //down
        aIndexBig = 0;
        aIndexSmall = 0;
        break;
    case 1: //up
        aIndexBig = 2;
        aIndexSmall = 1;
        break;
    case 2: //left
        aIndexBig = 7;
        aIndexSmall = 3;
        break;
    case 3: //right
        aIndexBig = 4;
        aIndexSmall = 2;
        break;
    }
    
    //find index offset based on walk state
    u32 walkDist = pd->entity.p.walkDist;
    u8 walkingOffset = (walkDist >> 4) % 2;
    if(pd->entity.p.dir==2 || pd->entity.p.dir==3) {
        walkingOffset = (walkDist >> 4) % 4;
        if(walkingOffset==2) walkingOffset = 0;
        if(walkingOffset==3) walkingOffset = 2;
    }
    
    bool swimming = isWater(pd->entity.level, pd->entity.x>>4, pd->entity.y>>4);
    
    //render water anim when swimming
    if (swimming) {
        renderc(xo, yo + 5, 48, 160 + (((pd->entity.p.swimTimer >> 4) & 1) << 3), 16, 8, 0);
    }
    
    bool accFront[] = PLAYER_SPRITE_ACCS_FRONT;
    
    //render the different parts
    //accs (back)
    if(aIndexSmall==0 && !accFront[pd->sprite.accs]) renderp(xo, yo, (42+aIndexSmall)*16, pd->sprite.accs*16);
    //legs
    if(!swimming) {
        renderp(xo, yo, (0+aIndexBig+walkingOffset)*16, pd->sprite.legs*16);
    }
    //body
    renderp(xo, yo, (10+aIndexBig+walkingOffset)*16, pd->sprite.body*16);
    //arms (normal)
    if(!(pd->entity.p.isCarrying)) {
        renderp(xo, yo, (20+aIndexBig+walkingOffset)*16, pd->sprite.arms*16);
    }
    //head
    renderp(xo, yo, (30+aIndexSmall)*16, pd->sprite.head*16);
    //eyes
    renderp(xo, yo, (34+aIndexSmall)*16, pd->sprite.eyes*16);
    //arms (carrying)
    if(pd->entity.p.isCarrying) {
        renderp(xo, yo, (38+aIndexSmall)*16, pd->sprite.arms*16);
    } 
    //accs (front)
    if(aIndexSmall!=0 || accFront[pd->sprite.accs]) renderp(xo, yo, (42+aIndexSmall)*16, pd->sprite.accs*16);
    
    
    //furniture
    if (pd->entity.p.isCarrying) {
        renderFurniture(pd->activeItem->id, xo, yo - 12);
    }

    //attack animation (other directios)
    if (pd->entity.p.attackTimer > 0) {
        switch (pd->entity.p.dir) {
        case 0:
            renderc(xo - pd->entity.p.ax, yo - pd->entity.p.ay + 12, 16, 168, 16, 8, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 4, yo + 12);
            break;
        case 2:
            renderc(xo - pd->entity.p.ax - 4, yo - pd->entity.p.ay, 32, 160, 8, 16, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo - 4, yo + 4);
            break;
        case 3:
            renderc(xo - pd->entity.p.ax + 12, yo - pd->entity.p.ay, 40, 160, 8, 16, 0);
            renderItemIcon(pd->activeItem->id, pd->activeItem->countLevel, xo + 12, yo + 4);
            break;
        }
    }
}

void renderWeather(u8 level, int xScroll, int yScroll) {
    if(level==1) {
        if(worldData.season==3) {
            int xp = -128 + ((syncTickCount>>2) - xScroll*2)%128;
            int yp = -128 + ((syncTickCount>>1) - yScroll*2)%128;
            int xp2 = 0 - ((syncTickCount>>2) + xScroll*2)%128;
            int yp2 = -128 + ((syncTickCount>>1)+64 - yScroll*2)%128;
            int xt;
            int yt;
            for(xt=0; xt<4; ++xt) {
                for(yt=0; yt<3; ++yt) {
                    sf2d_draw_texture_part_scale(icons, xp + xt*128, yp + yt*128, 192, 0, 64, 64, 2, 2);
                    sf2d_draw_texture_part_scale(icons, xp2 + xt*128, yp2 + yt*128, 192, 0, 64, 64, 2, 2);
                }
            }
        }
        
        if(worldData.rain) {
            int xp = -((xScroll*2)%128);
            int yp = -128 + ((syncTickCount<<2) - yScroll*2)%128;
            int xp2 = -((xScroll*2+8)%128);
            int yp2 = -128 + ((syncTickCount<<1)+64 - yScroll*2)%128;
            int xt;
            int yt;
            for(xt=0; xt<4; ++xt) {
                for(yt=0; yt<3; ++yt) {
                    sf2d_draw_texture_part_scale(icons, xp + xt*128, yp + yt*128, 128, 0, 64, 64, 2, 2);
                    sf2d_draw_texture_part_scale(icons, xp2 + xt*128, yp2 + yt*128, 128, 0, 64, 64, 2, 2);
                }
            }
        }
    }
}

void renderDayNight(PlayerData *pd) {
    if(pd->entity.level==1 && (worldData.daytime<6000 || worldData.daytime>18000)) {
        int color1 = 0x000C0C0C;
        int color2 = 0x00100C0C;
        int alpha1 = 0x88;
        int alpha2 = 0xDD;
        
        if(worldData.daytime>5000 && worldData.daytime<6000) {
            alpha2 = (alpha2 * (1000-(worldData.daytime-5000)))/1000;
            alpha1 = (alpha1 * (1000-(worldData.daytime-5000)))/1000;
        } else if(worldData.daytime>18000 && worldData.daytime<19000) {
            alpha1 = (alpha1 * (worldData.daytime-18000))/1000;
            alpha2 = (alpha2 * (worldData.daytime-18000))/1000;
        }
        
        color1 = color1 | (alpha1 << 24);
        color2 = color2 | (alpha2 << 24);
        
        sf2d_draw_rectangle(0, 0, 400, 240, color1); //You might think "real" black would be better, but it actually looks better that way
        renderLightsToStencil(pd, true, true, false);
        sf2d_draw_rectangle(0, 0, 400, 240, color2); //You might think "real" black would be better, but it actually looks better that way
        resetStencil();
    }
}

void renderFurniture(int itemID, int x, int y) {
    switch (itemID) {
    case ITEM_ANVIL:
        render16(x, y, 64, 128, 0);
        break;
    case ITEM_CHEST:
        render16(x, y, 80, 128, 0);
        break;
    case ITEM_OVEN:
        render16(x, y, 96, 128, 0);
        break;
    case ITEM_FURNACE:
        render16(x, y, 112, 128, 0);
        break;
    case ITEM_WORKBENCH:
        render16(x, y, 128, 128, 0);
        break;
    case ITEM_LANTERN:
        render16(x, y, 144, 128, 0);
        break;
    case ITEM_LOOM:
        render16(x, y, 224, 128, 0);
        break;
    case ITEM_ENCHANTER:
        render16(x, y, 240, 128, 0);
        break;
    }
}

char ertxt[20];
void renderEntity(Entity* e, int x, int y) {
    switch (e->type) {
    case ENTITY_ITEM:
        if (e->entityItem.age >= 520)
            if (e->entityItem.age / 6 % 2 == 0)
                return;
        renderItemIcon(e->entityItem.item.id, e->entityItem.item.countLevel,
                x - 4, y - 4 - (int) e->entityItem.zz);
        break;
    case ENTITY_FURNITURE:
        renderFurniture(e->entityFurniture.itemID, x - 8, y - 8);
        break;
    case ENTITY_ZOMBIE:
        switch (e->hostile.dir) {
        case 0: // down
            render16b(x - 8, y - 8, 64, 112, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            render16b(x - 8, y - 8, 80, 112, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            render16b(x - 8, y - 8, 96 + (((e->hostile.walkDist >> 4) & 1) << 4), 112, 1, e->hostile.color);
            break;
        case 3: // right
            render16b(x - 8, y - 8, 96 + (((e->hostile.walkDist >> 4) & 1) << 4), 112, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_SKELETON:
        switch (e->hostile.dir) {
        case 0: // down
            render16b(x - 8, y - 8, 0, 80, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            render16b(x - 8, y - 8, 16, 80, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            render16b(x - 8, y - 8, 32 + (((e->hostile.walkDist >> 4) & 1) << 4), 80, 1, e->hostile.color);
            break;
        case 3: // right
            render16b(x - 8, y - 8, 32 + (((e->hostile.walkDist >> 4) & 1) << 4), 80, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_KNIGHT:
        switch (e->hostile.dir) {
        case 0: // down
            render16b(x - 8, y - 8, 64, 80, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 1: // up
            render16b(x - 8, y - 8, 80, 80, ((e->hostile.walkDist >> 4) & 1) == 0 ? 0 : 1, e->hostile.color);
            break;
        case 2: // left
            render16b(x - 8, y - 8, 96 + (((e->hostile.walkDist >> 4) & 1) << 4), 80, 1, e->hostile.color);
            break;
        case 3: // right
            render16b(x - 8, y - 8, 96 + (((e->hostile.walkDist >> 4) & 1) << 4), 80, 0, e->hostile.color);
            break;
        }
        break;
    case ENTITY_SLIME:
        render16b(x - 8, y - 8 - (e->hostile.randWalkTime > 0 ? 4 : 0),
                128 + (e->hostile.randWalkTime > 0 ? 16 : 0), 112, 0, e->hostile.color);
        break;
    case ENTITY_AIRWIZARD:
        e->wizard.spriteAdjust = 0;
        if (e->wizard.health < 200) {
            if (syncTickCount / 4 % 3 < 2)
                e->wizard.spriteAdjust = 16;
        } else if (e->wizard.health < 1000) {
            if (syncTickCount / 5 % 4 < 2)
                e->wizard.spriteAdjust = 16;
        }
        switch (e->wizard.dir) {
        case 0: // down
            render16(x - 8, y - 8, 160, 112 + e->wizard.spriteAdjust,
                    ((e->wizard.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 1: // up
            render16(x - 8, y - 8, 176, 112 + e->wizard.spriteAdjust,
                    ((e->wizard.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 2: // left
            render16(x - 8, y - 8, 192 + (((e->wizard.walkDist >> 4) & 1) << 4),
                    112 + e->wizard.spriteAdjust, 1);
            break;
        case 3: // right
            render16(x - 8, y - 8, 192 + (((e->wizard.walkDist >> 4) & 1) << 4),
                    112 + e->wizard.spriteAdjust, 0);
            break;
        }
        break;
    case ENTITY_PASSIVE:
        switch (e->passive.dir) {
        case 0: // down
            render16(x - 8, y - 8, (e->passive.mtype*64) + 0, 96, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 1: // up
            render16(x - 8, y - 8, (e->passive.mtype*64) + 16, 96, ((e->passive.walkDist >> 4) & 1) == 0 ? 0 : 1);
            break;
        case 2: // left
            render16(x - 8, y - 8, (e->passive.mtype*64) + 32 + (((e->passive.walkDist >> 4) & 1) << 4), 96, 1);
            break;
        case 3: // right
            render16(x - 8, y - 8, (e->passive.mtype*64) + 32 + (((e->passive.walkDist >> 4) & 1) << 4), 96, 0);
            break;
        }
        break;
    case ENTITY_TEXTPARTICLE:
        x -= offsetX;
        y -= offsetY;
        renderTextColorSized(e->textParticle.text, x + 1,
                y - (int) e->textParticle.zz + 1, 2, 0xFF);
        renderTextColorSized(e->textParticle.text, x,
                y - (int) e->textParticle.zz, 2, e->textParticle.color);
        break;
    case ENTITY_SMASHPARTICLE:
        render16(x, y, 0, 160, 0);
        break;
    case ENTITY_SPARK:
        if (e->spark.age >= 200)
            if (e->spark.age / 6 % 2 == 0)
                return;
        renderr(x, y, 200, 152, 0, e->spark.age * 0.0349);
        break;
    case ENTITY_DRAGON:
        switch (e->dragon.dir) {
        case 0: // down
            render32(x - 16, y - 16, 0+(e->dragon.animTimer/4 * 32), 256, 2);
            break;
        case 1: // up
            render32(x - 16, y - 16, 0+(e->dragon.animTimer/4 * 32), 256, 0);
            break;
        case 2: // left
            render32(x - 16, y - 16, 0+(e->dragon.animTimer/4 * 32), 288, 1);
            break;
        case 3: // right
            render32(x - 16, y - 16, 0+(e->dragon.animTimer/4 * 32), 288, 0);
            break;
        }
        break;
    case ENTITY_DRAGONPROJECTILE: 
        if(e->dragonFire.type==0) {
            renderr(x, y, 0, 320, 0, e->dragonFire.age * 0.349);
        } else {
            render(x, y, 8, 320 + (e->dragonFire.age/10)*8, 0);
        }
        break;
    case ENTITY_MAGIC_PILLAR:
        render16(x - 8, y - 8, 16, 320, 0);
        break;
    case ENTITY_ARROW:
        if (e->arrow.age >= 200)
            if (e->arrow.age / 6 % 2 == 0)
                return;
        
        int abits = 0;
        int ayp = 168;
        if(e->arrow.xa<0) {
            abits += 1;
        }
        if(e->arrow.ya<0) {
            ayp += 8;
        }
        if(e->arrow.ya>0) {
            ayp += 8;
            abits += 2;
        }
        
        switch (e->arrow.itemID) {
        case ITEM_ARROW_WOOD:
            render(x-2, y-2, 72, ayp, abits);
            break;
        case ITEM_ARROW_STONE:
            render(x-2, y-2, 80, ayp, abits);
            break;
        case ITEM_ARROW_IRON:
            render(x-2, y-2, 88, ayp, abits);
            break;
        case ITEM_ARROW_GOLD:
            render(x-2, y-2, 96, ayp, abits);
            break;
        case ITEM_ARROW_GEM:
            render(x-2, y-2, 104, ayp, abits);
            break;
        }
        break;
    case ENTITY_GLOWWORM:
        render(x-4, y-4, 224, 112, 0);
        break;
    case ENTITY_NPC:
        render16(x - 8, y - 8, (e->npc.type*16) + 0, 64, 0);
    }
}

void renderEntities(u8 level, int x, int y, EntityManager* em) {
    int i;
    for (i = 0; i < em->lastSlot[level]; ++i) {
        Entity e = em->entities[level][i];
        if (e.x > x - 200 && e.y > y - 125 && e.x < x + 200 && e.y < y + 125)
            renderEntity(&e, e.x, e.y);
    }
}

void renderItemList(Inventory * inv, int xo, int yo, int x1, int y1,
        int selected) {
    // If lastSlot is 0, then there are no items are in the inventory.
    bool drawCursor = true;
    if (selected < 0) {
        drawCursor = false;
        selected = 0;
    }
    int w = x1 - xo;
    int h = y1 - yo - 2;
    int i1 = inv->lastSlot;
    if (i1 > h)
        i1 = h;
    int io = selected - h / 2;
    if (io > inv->lastSlot - h)
        io = inv->lastSlot - h;
    if (io < 0)
        io = 0;

    int i;
    for (i = 0; i < i1; ++i)
        renderItemWithText(&inv->items[i + io], (1 + xo) << 4,
                (i + 1 + yo) << 4);

    if (drawCursor) {
        int yy = selected + 1 - io + yo;
        sf2d_draw_rectangle((xo << 4) - (offsetX << 1),
                (yy << 4) - (offsetY << 1), 12, 12, 0xFF);
        renderText(">", (xo << 4), yy << 4);
        sf2d_draw_rectangle(((xo + w) << 4) - 12 - (offsetX << 1),
                (yy << 4) - (offsetY << 1), 12, 12, 0xFF);
        renderText("<", ((xo + w) << 4) - 12, yy << 4);
    }
}

void renderRecipes(RecipeManager * r, int xo, int yo, int x1, int y1, int selected) {
    int size = r->size;
    if (size < 1)
        return;
    if (selected < 0)
        selected = 0;
    int w = x1 - xo;
    int h = y1 - yo - 2;
    int i1 = size;
    if (i1 > h)
        i1 = h;
    int io = selected - h / 2;
    if (io > size - h)
        io = size - h;
    if (io < 0)
        io = 0;

    int i, col;
    for (i = 0; i < i1; ++i) {
        int x = (1 + xo) << 4, y = (i + 1 + yo) << 4;
        renderItemIcon(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel, x >> 1, y >> 1);
        if (r->recipes[i + io].canCraft)
            col = 0xFFFFFFFF;
        else
            col = 0xFF7F7F7F;
        if(r->recipes[i + io].itemAmountLevel==1) {
            renderTextColor(itemGetName(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel), x + 18, y + 2, col);
        } else {
            renderTextColor(itemGetNameWithCount(r->recipes[i + io].itemResult, r->recipes[i + io].itemAmountLevel), x + 18, y + 2, col);
        }
    }

    int yy = selected + 1 - io + yo;
    sf2d_draw_rectangle(xo << 4, yy << 4, 12, 12, 0xFF);
    renderText(">", xo << 4, yy << 4);
    sf2d_draw_rectangle(((xo + w) << 4) - 12, yy << 4, 12, 12, 0xFF);
    renderText("<", ((xo + w) << 4) - 12, yy << 4);
}

void renderItemWithText(Item* item, int x, int y) {
    renderItemIcon(item->id, item->countLevel, x >> 1, y >> 1);
    if (itemIsSingle(item->id, item->countLevel))
        renderText(itemGetNameWithCount(item->id, item->countLevel), x + 18, y + 2);
    else
        renderTextColorSpecial(itemGetNameWithCount(item->id, item->countLevel), x + 18,
                y + 2, 0xFFD2D2D2, 0xFFFFFFFF);
}

void renderItemStuffWithText(int itemID, int itemCL, bool onlyOne, int x, int y) {
    renderItemIcon(itemID, itemCL, x >> 1, y >> 1);
    if (onlyOne)
        renderText(itemGetNameWithCount(itemID, itemCL), x + 18, y + 2);
    else
        renderTextColorSpecial(itemGetNameWithCount(itemID, itemCL), x + 18,
                y + 2, 0xFFD2D2D2, 0xFFFFFFFF);
}

/* For bottom screen */ 
void renderItemWithTextCentered(Item* item, int width, int y) {
    char * tn = itemGetNameWithCount(item->id, item->countLevel);
    int x = (width - ((strlen(tn) + 2) * 12))/2;
    
    renderItemIcon(item->id, item->countLevel, x >> 1, y >> 1);
    
    if (itemIsSingle(item->id, item->countLevel))
        renderText(itemGetNameWithCount(item->id, item->countLevel), x + 18, y + 2);
    else
        renderTextColorSpecial(itemGetNameWithCount(item->id, item->countLevel), x + 18,
                y + 2, 0xFFD2D2D2, 0xFFFFFFFF);
}

void renderItemIcon(int itemID, int countLevel, int x, int y) {
    int xd;
    int yd;
    switch (itemID) {
    //TODO: This should not be here, somehow handle it in data?
    case TOOL_MAGIC_COMPASS:
        xd = worldData.compassData[getLocalPlayer()->entity.level][0] - (getLocalPlayer()->entity.x>>4);
        yd = worldData.compassData[getLocalPlayer()->entity.level][1] - (getLocalPlayer()->entity.y>>4);
        if(abs(yd)>abs(xd)) {
            if(yd<0) render(x, y, itemGetIconX(itemID, countLevel) + 0, itemGetIconY(itemID, countLevel), 0);
            else render(x, y, itemGetIconX(itemID, countLevel) + 8, itemGetIconY(itemID, countLevel), 0);
        } else {
            if(xd<0) render(x, y, itemGetIconX(itemID, countLevel) + 16, itemGetIconY(itemID, countLevel), 0);
            else render(x, y, itemGetIconX(itemID, countLevel) + 24, itemGetIconY(itemID, countLevel), 0);
        }
        break;
    default:
        render(x, y, itemGetIconX(itemID, countLevel), itemGetIconY(itemID, countLevel), 0);
        break;
    }
}
