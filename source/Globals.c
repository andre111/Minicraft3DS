#include "Globals.h"

#include "Synchronizer.h"

char versionText[34] = "Version 1.5.2";
char fpsstr[34];
u8 currentMenu = 0;

void addItemsToWorld(Item item, s8 level, int x, int y, int count){
    int i;
    for(i = 0; i < count; ++i) addEntityToList(newItemEntity(item, x, y, level), &eManager);
}

bool intersects(Entity e,int x0, int y0, int x1, int y1) {
	return !(e.x + e.xr < x0 || e.y + e.yr < y0 || e.x - e.xr > x1 || e.y - e.yr > y1);
}

int getEntities(Entity **result, s8 level, int x0, int y0, int x1, int y1) {
    int i, last = 0;
	for (i = 0; i < eManager.lastSlot[level]; ++i) {
        Entity* e = &eManager.entities[level][i];
		if (intersects(*e, x0, y0, x1, y1)){
            result[last] = e;
            ++last;
        }
	}
	return last;
}

void removeSimilarElements(Entity **arr1, int arr1Size, Entity **arr2, int arr2Size){
    int i,j;
    for(i=0;i<arr1Size;++i){
        for(j=0;j<arr2Size;++j){
            if(arr1[i] == arr2[j]) arr2[j] = NULL;
        }
    }
}

bool move2(Entity* e, int xa, int ya) {
	if (xa != 0 && ya != 0) return false;
	int xto0 = ((e->x) - e->xr) >> 4;
	int yto0 = ((e->y) - e->yr) >> 4;
	int xto1 = ((e->x) + e->xr) >> 4;
	int yto1 = ((e->y) + e->yr) >> 4;

	int xt0 = ((e->x + xa) - e->xr) >> 4;
	int yt0 = ((e->y + ya) - e->yr) >> 4;
	int xt1 = ((e->x + xa) + e->xr) >> 4;
	int yt1 = ((e->y + ya) + e->yr) >> 4;
	bool blocked = false;
	int xt,yt;
	for (yt = yt0; yt <= yt1; ++yt)
		for (xt = xt0; xt <= xt1; ++xt) {
			if (xt >= xto0 && xt <= xto1 && yt >= yto0 && yt <= yto1) continue;
			entityTileInteract(e, getTile(e->level, xt, yt), e->level, xt, yt);
			if (!e->canPass && tileIsSolid(getTile(e->level, xt, yt), e)) {
				blocked = true;
				return false;
			}
		}
	if (blocked) return false;

	Entity * wasInside[eManager.lastSlot[e->level]];
	Entity * isInside[eManager.lastSlot[e->level]];
    int wasSize = getEntities(wasInside, e->level, e->x - e->xr, e->y - e->yr, e->x + e->xr, e->y + e->yr);
	int isSize = getEntities(isInside, e->level, e->x + xa - e->xr, e->y + ya - e->yr, e->x + xa + e->xr, e->y + ya + e->yr);
	int i;
		
	for (i = 0; i < isSize; ++i) {
		Entity * ent = isInside[i];
		if (ent == e || ent == NULL) continue;
		EntityVsEntity(e, ent);
	}
	if(e->type != ENTITY_PLAYER){
        for(int i=0; i<playerCount; i++) {
            if(players[i].entity.level==e->level && intersects(players[i].entity, e->x + xa - e->xr, e->y + ya - e->yr, e->x + xa + e->xr, e->y + ya + e->yr)){
                EntityVsEntity(e, &(players[i].entity));
            }
        }
    }
	removeSimilarElements(wasInside, wasSize, isInside, isSize);
		
	for (i = 0; i < isSize; ++i) {
		Entity * ent = isInside[i];
		if (ent == e || ent == NULL) continue;
		if (EntityBlocksEntity(e, ent)) return false;
	}
    
    if(e->x + xa > 0 && e->x + xa < 2048) e->x += xa;
	if(e->y + ya > 0 && e->y + ya < 2048) e->y += ya;
	return true;
}

bool move(Entity* e, int xa, int ya) {
	if (xa != 0 || ya != 0) {
		bool stopped = true;
		if (xa != 0 && move2(e, xa, 0)) stopped = false;
		if (ya != 0 && move2(e, 0, ya)) stopped = false;
		return !stopped;
	}
	return true;
}

bool moveMob(Entity* e, int xa, int ya){
    if (e->xKnockback < 0) {
        move2(e, -1, 0);
        e->xKnockback++;
    }
    if (e->xKnockback > 0) {
        move2(e, 1, 0);
        e->xKnockback--;
    }
    if (e->yKnockback < 0) {
        move2(e, 0, -1);
        e->yKnockback++;
    }
    if (e->yKnockback > 0) {
        move2(e, 0, 1);
        e->yKnockback--;
    }
    if (e->hurtTime > 0) return true;
    return move(e, xa, ya);
}

void hurtEntity(Entity *e, int damage, int dir, u32 hurtColor, Entity *damager){
    if (TESTGODMODE && e->type==ENTITY_PLAYER) return;
    if (e->hurtTime > 0) return;
	playSoundPositioned(snd_monsterHurt, e->level, e->x, e->y);

    char hurtText[11];
    sprintf(hurtText, "%d", damage);
    addEntityToList(newTextParticleEntity(hurtText, hurtColor, e->x, e->y, e->level), &eManager);
    
    int i;
    
    // In hindsight I should've made a generic Mob struct, but whatever. ¯\_(-.-)_/¯ 
    switch(e->type){
	   case ENTITY_PLAYER: 
            e->p.health -= damage; 
            if(e->p.health < 1){ 
                playSoundPositioned(snd_bossdeath, e->level, e->x, e->y);
                e->p.endTimer = 60;
                e->p.isDead = true;
                e->hurtTime = 10;
                return;
            }
       break;
	   case ENTITY_ZOMBIE: 
	   case ENTITY_SKELETON:
	   case ENTITY_KNIGHT:
            e->hostile.health -= damage; 
            if(e->hostile.health < 1){ 
                if(e->type == ENTITY_ZOMBIE) {
					addItemsToWorld(newItem(ITEM_FLESH,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
				} else if(e->type == ENTITY_SKELETON) {
					addItemsToWorld(newItem(ITEM_BONE,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
					if(rand()%2==0) addItemsToWorld(newItem(ITEM_ARROW_STONE,1), e->level, e->x+8, e->y+8, 1);
				} else if(e->type == ENTITY_KNIGHT) {
					addItemsToWorld(newItem(ITEM_IRONINGOT,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
				}
                if(damager!=NULL && damager->type==ENTITY_PLAYER) damager->p.data->score += 50 * (e->hostile.lvl + 1);
                removeEntityFromList(e, e->level, &eManager);
                if(e->level != 5) trySpawn(3, e->level);
                return;
            }
       break;
	   case ENTITY_SLIME: 
            e->slime.health -= damage; 
            if(e->slime.health < 1){ 
                addItemsToWorld(newItem(ITEM_SLIME,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
                if(damager!=NULL && damager->type==ENTITY_PLAYER) damager->p.data->score += 25 * (e->slime.lvl + 1);
                removeEntityFromList(e, e->level, &eManager);
                if(e->level != 5) trySpawn(3, e->level);
                return;
            }
        break;
	   case ENTITY_AIRWIZARD: 
            e->wizard.health -= damage; 
            airWizardHealthDisplay = e->wizard.health;
            if(e->wizard.health < 1){ 
				addItemsToWorld(newItem(ITEM_MAGIC_DUST,1), e->level, e->x+8, e->y+8, (rand()%2) + 2);
                removeEntityFromList(e,e->level,&eManager);
                playSound(snd_bossdeath);
                
                for(i=0; i<playerCount; i++) {
                    players[i].score += 1000;
                    if(!players[i].entity.p.hasWonSaved) players[i].entity.p.endTimer = 60;
                    if(!players[i].entity.p.hasWonSaved) players[i].entity.p.hasWon = true;
                    players[i].entity.p.hasWonSaved = true;
                }
                return;
            }
        break;
		case ENTITY_PASSIVE: 
            e->passive.health -= damage; 
            if(e->passive.health < 1){ 
				if(e->passive.mtype==0) {
					addItemsToWorld(newItem(ITEM_WOOL,1), e->level, e->x+8, e->y+8, (rand()%3) + 1);
				} else if(e->passive.mtype==1) {
					addItemsToWorld(newItem(ITEM_PORK_RAW,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
				} else if(e->passive.mtype==2) {
					addItemsToWorld(newItem(ITEM_BEEF_RAW,1), e->level, e->x+8, e->y+8, (rand()%2) + 1);
					if((rand()%2)==0) {
						addItemsToWorld(newItem(ITEM_LEATHER,1), e->level, e->x+8, e->y+8, 1);
					}
				}
                if(damager!=NULL && damager->type==ENTITY_PLAYER) damager->p.data->score += 10;
                removeEntityFromList(e, e->level, &eManager);
                if(e->level != 5) trySpawn(3, e->level);
                return;
            }
		break;
		case ENTITY_DRAGON: 
            e->dragon.health -= damage; 
            if(e->dragon.health < 1){ 
				addItemsToWorld(newItem(ITEM_DRAGON_EGG,1), e->level, e->x+8, e->y+8, 1);
				addItemsToWorld(newItem(ITEM_DRAGON_SCALE,1), e->level, e->x+8, e->y+8, (rand()%11) + 10);
                removeEntityFromList(e, e->level, &eManager);
                playSound(snd_bossdeath);
                for(i=0; i<playerCount; i++) {
                    players[i].score += 1000;
                }
                return;
            }
        break;
    }
	
	switch(dir){
        case -1:
            switch(e->type){
                case ENTITY_PLAYER:
                    switch(e->p.dir){
	                   case 0: e->yKnockback = -10; break;
	                   case 1: e->yKnockback = +10; break;
	                   case 2: e->xKnockback = +10; break;
	                   case 3: e->xKnockback = -10; break;
                    } 
                break;
                case ENTITY_ZOMBIE:
				case ENTITY_SKELETON:
				case ENTITY_KNIGHT:
                    switch(e->hostile.dir){
	                   case 0: e->yKnockback = -10; break;
	                   case 1: e->yKnockback = +10; break;
	                   case 2: e->xKnockback = +10; break;
	                   case 3: e->xKnockback = -10; break;
                    } 
                break;
                case ENTITY_SLIME:
                    switch(e->slime.dir){
	                   case 0: e->yKnockback = -10; break;
	                   case 1: e->yKnockback = +10; break;
	                   case 2: e->xKnockback = +10; break;
	                   case 3: e->xKnockback = -10; break;
                    } 
                break;
				case ENTITY_PASSIVE:
                    switch(e->passive.dir){
	                   case 0: e->yKnockback = -10; break;
	                   case 1: e->yKnockback = +10; break;
	                   case 2: e->xKnockback = +10; break;
	                   case 3: e->xKnockback = -10; break;
                    } 
                break;
            }
        break;
        case 0: e->yKnockback = +10; break;
	    case 1: e->yKnockback = -10; break;
	    case 2: e->xKnockback = -10; break;
	    case 3: e->xKnockback = +10; break;
    }
	e->hurtTime = 10;
}

bool ItemVsEntity(PlayerData *pd, Item *item, Entity *e, int dir) {
    //TODO: To much duplicated code
    switch(item->id){
        case ITEM_POWGLOVE:
            if(e->type == ENTITY_FURNITURE){
                //Important: close all crafting windows using this furniture (only applies to chest) or else they will write invalid memory
                for(int i=0; i<playerCount; i++) {
                    if(players[i].curChestEntity==e) {
                        players[i].ingameMenu = MENU_NONE;
                    }
                }
                
                Item nItem = newItem(e->entityFurniture.itemID,0);
                if(e->entityFurniture.itemID == ITEM_CHEST) nItem.chestPtr = e->entityFurniture.inv;
                pushItemToInventoryFront(nItem, &(pd->inventory));
                
                removeEntityFromList(e, e->level, &eManager);
                pd->activeItem = &(pd->inventory.items[0]);
                pd->entity.p.isCarrying = true;
                return true;
            } break;
        case TOOL_AXE:
            switch(e->type){
				case ENTITY_PASSIVE:
                case ENTITY_ZOMBIE:
				case ENTITY_SKELETON:
				case ENTITY_KNIGHT:
                case ENTITY_SLIME:
                case ENTITY_AIRWIZARD:
				case ENTITY_DRAGON:
                    if(playerUseEnergy(pd, 4-item->countLevel)) hurtEntity(e, (item->countLevel + 1) * 2 + (rand()%4), dir, 0xFF0000FF, &(pd->entity));  
                    else hurtEntity(e, 1+rand()%3, dir, 0xFF0000FF, &(pd->entity));  
                return true;
				
				case ENTITY_MAGIC_PILLAR:
                    playSoundPositioned(snd_monsterHurt, e->level, e->x, e->y);
                    
					removeEntityFromList(e, e->level, &eManager);
				return true;
            } break;
        case TOOL_SWORD:
            switch(e->type){
				case ENTITY_PASSIVE:
                case ENTITY_ZOMBIE:
				case ENTITY_SKELETON:
				case ENTITY_KNIGHT:
                case ENTITY_SLIME:
                case ENTITY_AIRWIZARD:
				case ENTITY_DRAGON:
                    if(playerUseEnergy(pd, 4-item->countLevel)) hurtEntity(e, (item->countLevel+1)*3+(rand()%(2+item->countLevel*item->countLevel*2)), dir, 0xFF0000FF, &(pd->entity));   
                    else hurtEntity(e, 1+rand()%3, dir, 0xFF0000FF, &(pd->entity));     
                return true;
				
				case ENTITY_MAGIC_PILLAR:
                    playSoundPositioned(snd_monsterHurt, e->level, e->x, e->y);
                    
					removeEntityFromList(e, e->level, &eManager);
				return true;
            } break;
        case ITEM_NULL:
            switch(e->type){
				case ENTITY_PASSIVE:
                case ENTITY_ZOMBIE:
				case ENTITY_SKELETON:
				case ENTITY_KNIGHT:
                case ENTITY_SLIME:
                case ENTITY_AIRWIZARD:
				case ENTITY_DRAGON:
                    hurtEntity(e, 1+rand()%3, dir, 0xFF0000FF, &(pd->entity));
                return true;
				
				case ENTITY_MAGIC_PILLAR:
                    playSoundPositioned(snd_monsterHurt, e->level, e->x, e->y);
                    
					removeEntityFromList(e, e->level, &eManager);
				return true;
            } break;
    }
    return false;
}

void EntityVsEntity(Entity* e1, Entity* e2){
	int damage = 1;
    switch(e1->type){
        case ENTITY_PLAYER: playerEntityInteract(e1->p.data, e2); break;
        case ENTITY_ZOMBIE:
		case ENTITY_SKELETON:
		case ENTITY_KNIGHT:
            if(e2->type == ENTITY_PLAYER){ 
                if(e1->type == ENTITY_ZOMBIE) hurtEntity(e2, 2, e1->hostile.dir, 0xFFAF00FF, e1);
				else if(e1->type == ENTITY_SKELETON) hurtEntity(e2, 1, e1->hostile.dir, 0xFFAF00FF, e1);
				else if(e1->type == ENTITY_KNIGHT) hurtEntity(e2, 3, e1->hostile.dir, 0xFFAF00FF, e1);
                switch(e1->hostile.dir){
	                case 0: e1->yKnockback = -4; break;
	                case 1: e1->yKnockback = +4; break;
	                case 2: e1->xKnockback = +4; break;
	                case 3: e1->xKnockback = -4; break;
                } 
            }
        break;
        case ENTITY_SLIME:
            if(e2->type == ENTITY_PLAYER){
                hurtEntity(e2, 1, e1->slime.dir, 0xFFAF00FF, e1);
                switch(e1->slime.dir){
	                case 0: e1->yKnockback = -4; break;
	                case 1: e1->yKnockback = +4; break;
	                case 2: e1->xKnockback = +4; break;
	                case 3: e1->xKnockback = -4; break;
                } 
            }
        break;
        case ENTITY_AIRWIZARD:
            if(e2->type == ENTITY_PLAYER) hurtEntity(e2, 3, e1->wizard.dir, 0xFFAF00FF, e1);
        break;
        case ENTITY_SPARK:
            if(e2 != e1->spark.parent) hurtEntity(e2, 1, -1, 0xFFAF00FF, e1);
        break;
		case ENTITY_DRAGON:
		    if(e2->type == ENTITY_PLAYER) hurtEntity(e2, 3, e1->dragon.dir, 0xFFAF00FF, e1);
        break;
        case ENTITY_DRAGONPROJECTILE:
            if(e2 != e1->dragonFire.parent) hurtEntity(e2, 1, -1, 0xFFAF00FF, e1);
        break;
		case ENTITY_ARROW:
			switch(e1->arrow.itemID) {
			case ITEM_ARROW_WOOD: 
				damage = 1 + (rand()%3);
				break;
			case ITEM_ARROW_STONE: 
				damage = 2 + (rand()%4);
				break;
			case ITEM_ARROW_IRON: 
				damage = 8 + (rand()%9);
				break;
			case ITEM_ARROW_GOLD: 
				damage = 16 + (rand()%9);
				break;
			case ITEM_ARROW_GEM: 
				damage = 24 + (rand()%9);
				break;
			}
		
			if(e1->arrow.parent->type == ENTITY_PLAYER) {
				if(e2->type != ENTITY_PLAYER) {
					hurtEntity(e2, damage, -1, 0xFF0000FF, e1);
					removeEntityFromList(e1, e1->level, &eManager);
				}
			} else {
				if(e2->type == ENTITY_PLAYER) {
					hurtEntity(e2, damage, -1, 0xFFAF00FF, e1);
					removeEntityFromList(e1, e1->level, &eManager);
				}
			}
		break;
    }  
}

bool EntityBlocksEntity(Entity* e1, Entity* e2){
    switch(e1->type){
        case ENTITY_PLAYER:
        case ENTITY_FURNITURE: 
            switch(e2->type){
                case ENTITY_FURNITURE:
                case ENTITY_ZOMBIE:
				case ENTITY_SKELETON:
				case ENTITY_KNIGHT:
                case ENTITY_SLIME:
                case ENTITY_AIRWIZARD:
				case ENTITY_DRAGON:
                case ENTITY_PLAYER:
				case ENTITY_PASSIVE:
				case ENTITY_MAGIC_PILLAR:
                case ENTITY_NPC:
                    return true;
                break;
            }
        break;
    }  
    return false;
}

bool tileIsSolid(int tile, Entity * e){
    switch(tile){
        case TILE_ROCK: 
        case TILE_HARDROCK: 
		case TILE_MAGIC_BARRIER:
		case TILE_DUNGEON_WALL: 
            return true;
        case TILE_TREE: 
        case TILE_CACTUS: 
        case TILE_IRONORE: 
        case TILE_GOLDORE: 
        case TILE_GEMORE: 
        case TILE_CLOUDCACTUS: 
		case TILE_WOOD_WALL: 
		case TILE_STONE_WALL: 
		case TILE_IRON_WALL: 
		case TILE_GOLD_WALL: 
		case TILE_GEM_WALL: 
		case TILE_BOOKSHELVES: 
        case TILE_MUSHROOM_BROWN:
        case TILE_MUSHROOM_RED:
            if(e->type != ENTITY_DRAGON) return true;
		case TILE_LAVA: 
		case 255: 
			if(e->type != ENTITY_ARROW) return true;
        case TILE_WATER:
            if(e != NULL && !e->canSwim && e->type != ENTITY_ARROW) return true;
        case TILE_HOLE:
            if(e != NULL && e->type != ENTITY_PLAYER && e->type != ENTITY_ARROW) return true;
    }
    return false;
}

/* For minimap */
u32 getTileColor(int tile){
    switch(tile){
        case TILE_WATER: return SWAP_UINT32(waterColor[0]);
        case TILE_LAVA: return SWAP_UINT32(lavaColor[0]);
        case TILE_DIRT: return 0x826D6CFF;
        case TILE_ROCK: return SWAP_UINT32(rockColor[1]);
        case TILE_HARDROCK: return SWAP_UINT32(rockColor[3]);
        case TILE_GRASS: return SWAP_UINT32(grassColor);
        case TILE_TREE: return 0x007F00FF;
        case TILE_SAND: return SWAP_UINT32(sandColor);
        case TILE_CACTUS: return 0x009F00FF;
        case TILE_FLOWER: return SWAP_UINT32(grassColor);
        case TILE_IRONORE: return SWAP_UINT32(ironColor);
        case TILE_GOLDORE: return SWAP_UINT32(goldColor);
        case TILE_GEMORE: return SWAP_UINT32(gemColor);
        case TILE_CLOUD: return 0xFFFFFFFF;
        case TILE_CLOUDCACTUS: return 0xAFAFAFFF;
        case TILE_STAIRS_DOWN: return 0x9F9F9FFF;
        case TILE_STAIRS_UP: return 0x9F9F9FFF;
        case TILE_HOLE: return 0x383838FF;
		case TILE_WOOD_WALL: return SWAP_UINT32(woodColor);
		case TILE_STONE_WALL: return SWAP_UINT32(rockColor[1]);
		case TILE_IRON_WALL: return SWAP_UINT32(ironColor);
		case TILE_GOLD_WALL: return SWAP_UINT32(goldColor);
		case TILE_GEM_WALL: return SWAP_UINT32(gemColor);
		case TILE_DUNGEON_WALL: return SWAP_UINT32(dungeonColor[0]);
		case TILE_DUNGEON_FLOOR: return SWAP_UINT32(dungeonColor[1]);
		case TILE_MAGIC_BARRIER: return SWAP_UINT32(dungeonColor[0]);
		case TILE_BOOKSHELVES: return SWAP_UINT32(woodColor);
		case TILE_WOOD_FLOOR: return SWAP_UINT32(woodColor);
        case TILE_MYCELIUM: return SWAP_UINT32(myceliumColor);
        case TILE_MUSHROOM_BROWN: return SWAP_UINT32(mushroomColor);
        case TILE_MUSHROOM_RED: return SWAP_UINT32(mushroomColor);
        case TILE_ICE: return SWAP_UINT32(iceColor);
		
        default: return 0x111111FF;
    }
}

s8 itemTileInteract(int tile, PlayerData *pd, Item *item, s8 level, int x, int y, int px, int py, int dir){
    
    // Furniture items
    if(item->id > 27 && item->id < 51){
        if(!tileIsSolid(getTile(level, x, y), NULL)){
            addEntityToList(newFurnitureEntity(item->id, item->chestPtr, (x<<4)+8, (y<<4)+8, level), &eManager);
            removeItemFromCurrentInv(item);
            pd->activeItem = &noItem;
            return 2;
        }
        return 0;
    }
    
    switch(tile){
        case TILE_TREE:
            if(item->id == TOOL_AXE && playerUseEnergy(pd, 4-item->countLevel)){
                playerHurtTile(pd, tile, level, x, y, (rand()%10) + (item->countLevel) * 5 + 10, pd->entity.p.dir);
                return 1;
            } break;
        case TILE_ROCK:
        case TILE_HARDROCK:
            if(item->id == TOOL_PICKAXE && playerUseEnergy(pd, 4-item->countLevel)){
                playerHurtTile(pd, tile, level, x, y, (rand()%10) + (item->countLevel) * 5 + 10, pd->entity.p.dir);
                return 1;
            } break;
        case TILE_IRONORE:
        case TILE_GOLDORE:
        case TILE_GEMORE:
        case TILE_CLOUDCACTUS:
            if(item->id == TOOL_PICKAXE && playerUseEnergy(pd, 4-item->countLevel)){
                playerHurtTile(pd, tile, level, x, y, 1, pd->entity.p.dir);
                return 1;
            } break;
        case TILE_GRASS:
            if(item->id == TOOL_HOE && playerUseEnergy(pd, 4-item->countLevel)){ 
                setTile(TILE_FARM, level, x, y);
                return 1;
            }
            else if(item->id == ITEM_ACORN){ 
                setTile(TILE_SAPLING_TREE, level, x, y); --item->countLevel;
                return 1;
            } 
            else if(item->id == ITEM_FLOWER){ 
                setTile(TILE_FLOWER, level, x, y); --item->countLevel;
				setData(rand()%4, level, x, y); // determines mirroring.
                return 1;
            } 
			else if(item->id == ITEM_WALL_WOOD){ 
                setTile(TILE_WOOD_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_STONE){ 
                setTile(TILE_STONE_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_IRON){ 
                setTile(TILE_IRON_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_GOLD){ 
                setTile(TILE_GOLD_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_GEM){ 
                setTile(TILE_GEM_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
            else if(item->id == ITEM_BOOKSHELVES){ 
                setTile(TILE_BOOKSHELVES, level, x, y); --item->countLevel;
                setData(rand()%3, level, x, y); //determines sprite
                return 1;
            } 
            else if(item->id == TOOL_SHOVEL && playerUseEnergy(pd, 4-item->countLevel)){ 
                if(rand()%5==0) addItemsToWorld(newItem(ITEM_SEEDS,1), level, (x<<4)+8, (y<<4)+8, 1);
                setTile(TILE_DIRT, level, x, y); 
                return 1;
            } break;
        case TILE_SAND:
            if(item->id == ITEM_CACTUS){ 
                setTile(TILE_SAPLING_CACTUS, level, x, y); 
                --item->countLevel;
                return 1;
            }
            else if(item->id == TOOL_SHOVEL && playerUseEnergy(pd, 4-item->countLevel)){ 
                addItemsToWorld(newItem(ITEM_SAND,1), level, (x<<4)+8, (y<<4)+8, 1);
                setTile(TILE_DIRT, level, x, y); 
                return 1;
            } break;
        case TILE_DIRT:
            if(item->id == TOOL_HOE && pd->entity.level==1 && playerUseEnergy(pd, 4-item->countLevel)){ 
                setTile(TILE_FARM, level, x, y);
                return 1;
            }
			else if(item->id == ITEM_WALL_WOOD){ 
                setTile(TILE_WOOD_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_STONE){ 
                setTile(TILE_STONE_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_IRON){ 
                setTile(TILE_IRON_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_GOLD){ 
                setTile(TILE_GOLD_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
			else if(item->id == ITEM_WALL_GEM){ 
                setTile(TILE_GEM_WALL, level, x, y); --item->countLevel;
                return 1;
            } 
            else if(item->id == ITEM_BOOKSHELVES){ 
                setTile(TILE_BOOKSHELVES, level, x, y); --item->countLevel;
                setData(rand()%3, level, x, y); //determines sprite
                return 1;
            } 
            else if(item->id == ITEM_WOOD) {
                setTile(TILE_WOOD_FLOOR, level, x, y); --item->countLevel;
                return 1;
            }
			else if(item->id == ITEM_SAND){ 
                setTile(TILE_SAND, level, x, y); --item->countLevel;
                return 1;
            } 
            else if(item->id == TOOL_SHOVEL && playerUseEnergy(pd, 4-item->countLevel)){ 
                addItemsToWorld(newItem(ITEM_DIRT,1), level, (x<<4)+8, (y<<4)+8, 1);
                setTile(TILE_HOLE, level, x, y); 
                return 1;
            } break;
        case TILE_HOLE:
			if(item->id == ITEM_DIRT){
                setTile(TILE_DIRT, level, x, y);    
                --item->countLevel;
                return 1;
			}
			else if(item->id == TOOL_BUCKET && item->countLevel == 1 && playerUseEnergy(pd, 4)) {
				setTile(TILE_WATER, level, x, y);
				item->countLevel = 0;
            } 
			else if(item->id == TOOL_BUCKET && item->countLevel == 2 && playerUseEnergy(pd, 4)) {
				setTile(TILE_LAVA, level, x, y);
				item->countLevel = 0;
            } break;
        case TILE_WATER:
			if(item->id == ITEM_DIRT){
                setTile(TILE_DIRT, level, x, y);    
                --item->countLevel;
                return 1;
			}
			else if(item->id == TOOL_BUCKET && item->countLevel == 0 && playerUseEnergy(pd, 4)) {
				setTile(TILE_HOLE, level, x, y);
				item->countLevel = 1;
            } break;
        case TILE_LAVA:
            if(item->id == ITEM_DIRT){
                setTile(TILE_DIRT, level, x, y);    
                --item->countLevel;
                return 1;
            }
			else if(item->id == TOOL_BUCKET && item->countLevel == 0 && playerUseEnergy(pd, 4)) {
				setTile(TILE_HOLE, level, x, y);
				item->countLevel = 2;
            } break;
        case TILE_NULL:
            if(item->id == ITEM_CLOUD){
                setTile(TILE_CLOUD, level, x, y);    
                --item->countLevel;
                return 1;
            } break;
        case TILE_FARM:
            if(item->id == ITEM_SEEDS){
                setTile(TILE_WHEAT, level, x, y);
                setData(0, level, x, y);
                --item->countLevel;
                return 1;
            } break;
        case TILE_WHEAT:
            if(item->id == TOOL_HOE){
                if(getData(level, x, y) > -1){
                    int age = getData(level, x, y);
                    int count = (rand() % 2);
                    if(age >= 80) count = (rand()%2) + 1;
                    addItemsToWorld(newItem(ITEM_SEEDS,1), level, (x<<4)+8, (y<<4)+8, count);
                    count = 0;
			        if(age == 100) count = (rand()%3) + 2;
                    else if(age >= 80) count = (rand()%2) + 1;
                    addItemsToWorld(newItem(ITEM_WHEAT,1), level, (x<<4)+8, (y<<4)+8, count);
				    setTile(TILE_DIRT, level, x, y);
                }
            } break;
		case TILE_WOOD_WALL:
        case TILE_BOOKSHELVES:
            if(item->id == TOOL_AXE && playerUseEnergy(pd, 4-item->countLevel)){
                playerHurtTile(pd, tile, level, x, y, (rand()%10) + (item->countLevel) * 5 + 10, pd->entity.p.dir);
                return 1;
            } break;
		case TILE_STONE_WALL:
		case TILE_IRON_WALL:
		case TILE_GOLD_WALL:
		case TILE_GEM_WALL:
            if(item->id == TOOL_PICKAXE && playerUseEnergy(pd, 4-item->countLevel)){
                playerHurtTile(pd, tile, level, x, y, (rand()%10) + (item->countLevel) * 5 + 10, pd->entity.p.dir);
                return 1;
            } break;
        case TILE_WOOD_FLOOR:
            if(item->id == TOOL_AXE && playerUseEnergy(pd, 4-item->countLevel)){
                addItemsToWorld(newItem(ITEM_WOOD,1), level, (x<<4)+8, (y<<4)+8, 1);
                setTile(TILE_DIRT, level, x, y); 
            } break;
    }
    return 0;
}

void tickTile(s8 level, int x, int y){
    int tile = getTile(level, x, y);
    int data = getData(level, x, y);
    
    switch(tile){
        case TILE_SAPLING_TREE:
            if(worldData.season!=3) {
				setData(++data, level, x, y); 
				if(data>100){setData(0, level, x, y); setTile(TILE_TREE, level, x, y);}
			}
            break;
		case TILE_TREE:
			if(eManager.lastSlot[level]<800 && (worldData.daytime>18000 || worldData.daytime<5000) && rand()%800==0) {
				//check for nearby glowworms
                //TODO: This should really use getEntities
				int i = 0;
				for (i = 0; i < eManager.lastSlot[level]; ++i) {
					Entity * e = &eManager.entities[level][i];
					if(e->type==ENTITY_GLOWWORM && ((e->x)-(x<<4))*((e->x)-(x<<4))+((e->y)-(y<<4))*((e->y)-(y<<4)) < (2<<4)*(2<<4)) return;
				}
				addEntityToList(newGlowwormEntity((x<<4)+8, (y<<4)+8, level), &eManager);
			}
			break;
        case TILE_SAPLING_CACTUS:
            if(worldData.season!=3) {
				setData(++data, level, x, y); 
				if(data>100){setData(0, level, x, y); setTile(TILE_CACTUS, level, x, y);}
			}
            break;
        case TILE_WHEAT:
            if(data<100 && worldData.season!=3) setData(++data, level, x, y);
            break;
        case TILE_WATER:
            if(getTile(level, x+1, y)==TILE_HOLE) setTile(TILE_WATER, level, x+1, y);
            if(getTile(level, x-1, y)==TILE_HOLE) setTile(TILE_WATER, level, x-1, y);
            if(getTile(level, x, y+1)==TILE_HOLE) setTile(TILE_WATER, level, x, y+1);
            if(getTile(level, x, y-1)==TILE_HOLE) setTile(TILE_WATER, level, x, y-1);
            if(level==1 && worldData.season==3 && rand()%12==0) {
                if(getTile(level, x+1, y)!=TILE_WATER) setTile(TILE_ICE, level, x, y);
                if(getTile(level, x-1, y)!=TILE_WATER) setTile(TILE_ICE, level, x, y);
                if(getTile(level, x, y+1)!=TILE_WATER) setTile(TILE_ICE, level, x, y);
                if(getTile(level, x, y-1)!=TILE_WATER) setTile(TILE_ICE, level, x, y);
            }
            break;
        case TILE_LAVA:
            if(getTile(level, x+1, y)==TILE_HOLE) setTile(TILE_LAVA, level, x+1, y);
            if(getTile(level, x-1, y)==TILE_HOLE) setTile(TILE_LAVA, level, x-1, y);
            if(getTile(level, x, y+1)==TILE_HOLE) setTile(TILE_LAVA, level, x, y+1);
            if(getTile(level, x, y-1)==TILE_HOLE) setTile(TILE_LAVA, level, x, y-1);
			
			if(getTile(level, x+1, y)==TILE_WATER || getTile(level, x-1, y)==TILE_WATER || getTile(level, x, y+1)==TILE_WATER || getTile(level, x, y-1)==TILE_WATER) {
				setTile(TILE_ROCK, level, x, y);
			}
            break;
        case TILE_HOLE: // This makes water flow slightly faster than lava
            if(getTile(level, x+1, y)==TILE_WATER) setTile(TILE_WATER, level, x, y);
            if(getTile(level, x-1, y)==TILE_WATER) setTile(TILE_WATER, level, x, y);
            if(getTile(level, x, y+1)==TILE_WATER) setTile(TILE_WATER, level, x, y);
            if(getTile(level, x, y-1)==TILE_WATER) setTile(TILE_WATER, level, x, y);
            break;
        case TILE_GRASS:
            if(getTile(level, x+1, y)==TILE_DIRT) if((rand()%25) == 0) setTile(TILE_GRASS, level, x+1, y);
            if(getTile(level, x-1, y)==TILE_DIRT) if((rand()%25) == 0) setTile(TILE_GRASS, level, x-1, y);
            if(getTile(level, x, y+1)==TILE_DIRT) if((rand()%25) == 0) setTile(TILE_GRASS, level, x, y+1);
            if(getTile(level, x, y-1)==TILE_DIRT) if((rand()%25) == 0) setTile(TILE_GRASS, level, x, y-1);
            break;
		case TILE_SAND:
			if(data > 0) setData(--data, level, x, y);
			break;
		case TILE_CLOUD:
			if((rand()%24000)==0) setTile(TILE_CLOUDCACTUS, level, x, y);
			break;
		case TILE_MAGIC_BARRIER:
			data = 0;
			int i = 0;
			for (i = 0; i < eManager.lastSlot[level]; ++i) {
				Entity * e = &eManager.entities[level][i];
			
				if(e->type == ENTITY_MAGIC_PILLAR) {
					++data;
				}
			}
			if(data==0) setTile(TILE_DUNGEON_FLOOR, level, x, y);
            setData(rand()%2, level, x, y);
			break;
        case TILE_ICE:
            if(worldData.season!=3) {
                setTile(TILE_WATER, level, x, y);
            }
            break;
    }
    
}

void tickEntityItem(Entity* e){
    ++e->entityItem.age;
    if(e->entityItem.age == 630){ 
        removeEntityFromList(e, e->level, &eManager);
        /*
        Programming pro tip: 
            Remember to put a return statement after you remove the entity,
            or else your going to have a very bad time like I did.
        */
        return;
    }
    e->entityItem.xx += e->entityItem.xa;
    e->entityItem.yy += e->entityItem.ya;
    e->entityItem.zz += e->entityItem.za;
    if (e->entityItem.zz < 0) {
        e->entityItem.zz = 0;
        e->entityItem.za *= -0.5;
        e->entityItem.xa *= 0.6;
        e->entityItem.ya *= 0.6;
    }
    e->entityItem.za -= 0.15;
    int ox = e->x;
    int oy = e->y;
    int nx = (int) e->entityItem.xx;
    int ny = (int) e->entityItem.yy;
    int expectedx = nx - e->x;
    int expectedy = ny - e->y;
    move(e, nx - e->x, ny - e->y);
    int gotx = e->x - ox;
    int goty = e->y - oy;
    e->entityItem.xx += gotx - expectedx;
    e->entityItem.yy += goty - expectedy;
}

void tickEntityTextParticle(Entity* e){
    ++e->textParticle.age;
    if(e->textParticle.age == 60){ 
        removeEntityFromList(e, e->level, &eManager);
        return;
    }
    e->textParticle.xx += e->textParticle.xa;
    e->textParticle.yy += e->textParticle.ya;
    e->textParticle.zz += e->textParticle.za;
    if (e->textParticle.zz < 0) {
        e->textParticle.zz = 0;
        e->textParticle.za *= -0.5;
        e->textParticle.xa *= 0.6;
        e->textParticle.ya *= 0.6;
    }
    e->textParticle.za -= 0.15;
    e->x = (int) e->textParticle.xx;
    e->y = (int) e->textParticle.yy;
}

void tickEntity(Entity* e){
    PlayerData *nearestPlayer = getNearestPlayer(e->level, e->x, e->y);
    switch(e->type){
        case ENTITY_ITEM: tickEntityItem(e); return;
        case ENTITY_FURNITURE: return;
		case ENTITY_MAGIC_PILLAR: return;
        case ENTITY_ZOMBIE: 
		case ENTITY_SKELETON: 
		case ENTITY_KNIGHT: 
            if (e->hurtTime > 0) e->hurtTime--;
            if (e->hostile.randWalkTime == 0 && e->type != ENTITY_SKELETON && nearestPlayer!=NULL) {
			    int xd = nearestPlayer->entity.x - e->x;
			    int yd = nearestPlayer->entity.y - e->y;
				int dist = 50 * 50;
				if(e->type == ENTITY_KNIGHT) dist = 80 * 80;
				
				//charge player
			    if (xd * xd + yd * yd < dist) {
				    e->hostile.xa = 0;
				    e->hostile.ya = 0;
				    if (xd < 0) e->hostile.xa = -1; 
				    if (xd > 0) e->hostile.xa = +1;
				    if (yd < 0) e->hostile.ya = -1;
				    if (yd > 0) e->hostile.ya = +1;
			    }
		    }
		    
		    if(e->hostile.xa < 0) e->hostile.dir = 2;
		    else if(e->hostile.xa > 0) e->hostile.dir = 3;
		    if(e->hostile.ya < 0) e->hostile.dir = 1;
		    else if(e->hostile.ya > 0) e->hostile.dir = 0;
		    
			if(e->type == ENTITY_SKELETON) {
				--(e->hostile.randAttackTime);
				if(e->hostile.randAttackTime <= 0) {
					e->hostile.randAttackTime = 80 - (e->hostile.lvl * 5);
					
					int aitemID = ITEM_ARROW_WOOD;
					if(e->hostile.lvl >= 2) aitemID = ITEM_ARROW_STONE;
					
					//turn to player when attacking
                    if(nearestPlayer!=NULL) {
                        int xd = nearestPlayer->entity.x - e->x;
                        int yd = nearestPlayer->entity.y - e->y;
                        if(xd*xd > yd*yd) {
                            if (xd < 0) e->hostile.dir = 2;
                            if (xd > 0) e->hostile.dir = 3;
                        } else {
                            if (yd < 0) e->hostile.dir = 1;
                            if (yd > 0) e->hostile.dir = 0;
                        }
                    }
					
					switch(e->hostile.dir) {
					case 0:
						addEntityToList(newArrowEntity(e, aitemID, 0, 2, e->level), &eManager);
						break;
					case 1:
						addEntityToList(newArrowEntity(e, aitemID, 0, -2, e->level), &eManager);
						break;
					case 2:
						addEntityToList(newArrowEntity(e, aitemID, -2, 0, e->level), &eManager);
						break;
					case 3:
						addEntityToList(newArrowEntity(e, aitemID, 2, 0, e->level), &eManager);
						break;
					}
				}
			}
			
		    if(e->hostile.xa != 0 || e->hostile.ya != 0) e->hostile.walkDist++;

		    int speed = syncTickCount & 1;
		    if (!moveMob(e, e->hostile.xa * speed, e->hostile.ya * speed) || (rand()%100) == 0) {
			    e->hostile.randWalkTime = 60;
			    e->hostile.xa = ((rand()%3) - 1) * (rand()%2);
			    e->hostile.ya = ((rand()%3) - 1) * (rand()%2);
		    }
		    if (e->hostile.randWalkTime > 0) e->hostile.randWalkTime--;
            return;
        case ENTITY_SLIME: 
            if (e->hurtTime > 0) e->hurtTime--;
            
            if (!moveMob(e, e->slime.xa, e->slime.ya) || (rand()%10) == 0) {
                if (e->slime.jumpTime <= -10) {
			        e->slime.xa = ((rand()%3) - 1);
			        e->slime.ya = ((rand()%3) - 1);
			        
                    if(nearestPlayer!=NULL) {
                        int xd = nearestPlayer->entity.x - e->x;
                        int yd = nearestPlayer->entity.y - e->y;
                        if (xd * xd + yd * yd < 50 * 50) {
                            if (xd < 0) e->slime.xa = -1; 
                            if (xd > 0) e->slime.xa = +1;
                            if (yd < 0) e->slime.ya = -1;
                            if (yd > 0) e->slime.ya = +1;
                        }
                    }
			        
			        if (e->slime.xa != 0 || e->slime.ya != 0) e->slime.jumpTime = 10;
                }
		    }
		    
		    if(e->slime.xa < 0) e->slime.dir = 2;
		    else if(e->slime.xa > 0) e->slime.dir = 3;
		    if(e->slime.ya < 0) e->slime.dir = 1;
		    else if(e->slime.ya > 0) e->slime.dir = 0;
		    
		    if (e->slime.jumpTime > -10) e->slime.jumpTime--;
            if(e->slime.jumpTime == 0){
                e->slime.xa = 0;
                e->slime.ya = 0;
            }
            return;
        case ENTITY_AIRWIZARD: 
            if (e->hurtTime > 0) e->hurtTime--;
            
            if (e->wizard.attackDelay > 0) {
			    e->wizard.dir = (e->wizard.attackDelay - 45) / 4 % 4;
			    e->wizard.dir = (e->wizard.dir * 2 % 4) + (e->wizard.dir / 2);
			    if (e->wizard.attackDelay < 45) e->wizard.dir = 0;
			    e->wizard.attackDelay--;
			    if (e->wizard.attackDelay <= 0) {
				    e->wizard.attackType = 0;
				    if (e->wizard.health < 1000) e->wizard.attackType = 1;
				    if (e->wizard.health < 200) e->wizard.attackType = 2;
				    e->wizard.attackTime = 120;
			    }
			    return;
		    }
            
		    if (e->wizard.attackTime > 0) {
			    e->wizard.attackTime--;
			    float dir = e->wizard.attackTime * 0.25 * (e->wizard.attackTime % 2 * 2 - 1);
			    float speed = (0.7) + (e->wizard.attackType+1) * 0.2;
			    addEntityToList(newSparkEntity(e, cos(dir) * speed, sin(dir) * speed), &eManager);
			    return;
		    }
            
            if (e->wizard.randWalkTime == 0 && nearestPlayer!=NULL) {
			    int xd = nearestPlayer->entity.x - e->x;
			    int yd = nearestPlayer->entity.y - e->y;
			    int dist = xd * xd + yd * yd;
			    if (dist > 80 * 80) {
				    e->wizard.xa = 0;
				    e->wizard.ya = 0;
				    if (xd < 0) e->wizard.xa = -1; 
				    if (xd > 0) e->wizard.xa = +1;
				    if (yd < 0) e->wizard.ya = -1;
				    if (yd > 0) e->wizard.ya = +1;
			    } else if (dist < 24 * 24) {
				    e->wizard.xa = 0;
				    e->wizard.ya = 0;
				    if (xd < 0) e->wizard.xa = +1; 
				    if (xd > 0) e->wizard.xa = -1;
				    if (yd < 0) e->wizard.ya = +1;
				    if (yd > 0) e->wizard.ya = -1;
                }
		    }
		    
		    int wSpeed = (syncTickCount % 4) == 0 ? 0 : 1;
		    if (!moveMob(e, e->wizard.xa * wSpeed, e->wizard.ya * wSpeed) || (rand()%100) == 0) {
			    e->wizard.randWalkTime = 30;
			    e->wizard.xa = ((rand()%3) - 1) * (rand()%2);
			    e->wizard.ya = ((rand()%3) - 1) * (rand()%2);
		    }
		    
		    if(e->wizard.xa != 0 || e->wizard.ya != 0){ 
                e->wizard.walkDist++;
                awX = e->x;
                awY = e->y;
            }
		    
		    if(e->wizard.xa < 0) e->wizard.dir = 2;
		    else if(e->wizard.xa > 0) e->wizard.dir = 3;
		    if(e->wizard.ya < 0) e->wizard.dir = 1;
		    else if(e->wizard.ya > 0) e->wizard.dir = 0;
		    
		    if (e->wizard.randWalkTime > 0) {
			    e->wizard.randWalkTime--;
			    if (e->wizard.randWalkTime == 0 && nearestPlayer!=NULL) {
				    int xd = nearestPlayer->entity.x - e->x;
				    int yd = nearestPlayer->entity.y - e->y;
				    if (rand()%4 == 0 && xd * xd + yd * yd < 50 * 50) {
					    if (e->wizard.attackDelay == 0 && e->wizard.attackTime == 0) e->wizard.attackDelay = 120;
				    }
			    }
		    }
		    
            return;
        case ENTITY_SPARK: 
            e->spark.age++;
		    if (e->spark.age >= 260) {
			    removeEntityFromList(e, e->level, &eManager);
			    return;
		    }
		    e->spark.xx += e->spark.xa;
		    e->spark.yy += e->spark.ya;
		    e->x = (int) e->spark.xx;
		    e->y = (int) e->spark.yy;
		    
            if(nearestPlayer!=NULL && intersects(nearestPlayer->entity, e->x + e->spark.xa - e->xr, e->y + e->spark.ya - e->yr, e->x + e->spark.xa + e->xr, e->y + e->spark.ya + e->yr)){
                EntityVsEntity(e, &(nearestPlayer->entity));
			    removeEntityFromList(e, e->level, &eManager);
            }
            return;
		case ENTITY_DRAGON:
			if (e->hurtTime > 0) e->hurtTime--;
			
			e->dragon.animTimer++;
			if(e->dragon.animTimer>=4*4) {
				e->dragon.animTimer = 0;
			}
			
			//choose random attack
			if (e->dragon.attackDelay > 0) {
			    e->dragon.attackDelay--;
			    if (e->dragon.attackDelay <= 0) {
					e->dragon.attackType = rand()%2;
				    e->dragon.attackTime = 121;
				}
				return;
			}
			
			if (e->dragon.attackTime > 0) {
			    e->dragon.attackTime--;
				
				//turn to player when attacking
                if(nearestPlayer!=NULL) {
                    int xd = nearestPlayer->entity.x - e->x;
                    int yd = nearestPlayer->entity.y - e->y;
                    if(xd*xd > yd*yd) {
                        if (xd < 0) e->dragon.dir = 2;
                        if (xd > 0) e->dragon.dir = 3;
                    } else {
                        if (yd < 0) e->dragon.dir = 1;
                        if (yd > 0) e->dragon.dir = 0;
                    }
                }
				
				switch(e->dragon.attackType) {
				case 0: //Firebreathing
					if(e->dragon.attackTime%2 == 0) {
						float dfdir = 0;
						
						if(e->dragon.dir==0) dfdir = 1 * 3.141592 / 2;
						else if(e->dragon.dir==1) dfdir = 3 * 3.141592 / 2;
						else if(e->dragon.dir==2) dfdir = 2 * 3.141592 / 2;
						else if(e->dragon.dir==3) dfdir = 0 * 3.141592 / 2;
						
						dfdir += 0.03141592 * ((rand()%33) - 16);
						
						addEntityToList(newDragonFireEntity(e, e->dragon.attackType, e->x + cos(dfdir)*14, e->y + sin(dfdir)*14, cos(dfdir), sin(dfdir)), &eManager);
					}
				break;
				case 1: //Firering
					if(e->dragon.attackTime%20 == 0) {
						int ai = 0;
						for(ai = 0; ai < 16; ai++) {
							float ddir = (3.141592 * 2 / 16.0) * ai;
							float ddist = (140 - e->dragon.attackTime) / 2;
							
							addEntityToList(newDragonFireEntity(e, e->dragon.attackType, (e->x) + cos(ddir)*ddist, (e->y) + sin(ddir)*ddist, 0, 0), &eManager);
						}
					}
				break;
				}
				
				return;
			}
			
			//TODO - movement copied from airwizard, adjust to better fit dragon
			if (e->dragon.randWalkTime == 0 && nearestPlayer!=NULL) {
			    int xd = nearestPlayer->entity.x - e->x;
			    int yd = nearestPlayer->entity.y - e->y;
			    int dist = xd * xd + yd * yd;
			    if (dist > 64 * 64) {
				    e->dragon.xa = 0;
				    e->dragon.ya = 0;
				    if (xd < 0) e->dragon.xa = -1; 
				    if (xd > 0) e->dragon.xa = +1;
				    if (yd < 0) e->dragon.ya = -1;
				    if (yd > 0) e->dragon.ya = +1;
			    } else if (dist < 16 * 16) {
				    e->dragon.xa = 0;
				    e->dragon.ya = 0;
				    if (xd < 0) e->dragon.xa = +1; 
				    if (xd > 0) e->dragon.xa = -1;
				    if (yd < 0) e->dragon.ya = +1;
				    if (yd > 0) e->dragon.ya = -1;
                }
		    }
		    
		    int dSpeed = (syncTickCount % 4) == 0 ? 0 : 1;
		    if (!moveMob(e, e->dragon.xa * dSpeed, e->dragon.ya * dSpeed) || (rand()%120) == 0) {
			    e->dragon.randWalkTime = 30;
			    e->dragon.xa = ((rand()%3) - 1) * (rand()%2);
			    e->dragon.ya = ((rand()%3) - 1) * (rand()%2);
		    }
		    
		    if(e->dragon.xa != 0 || e->dragon.ya != 0){ 
                e->dragon.walkDist++;
            }
		    
		    if(e->dragon.xa < 0) e->dragon.dir = 2;
		    else if(e->dragon.xa > 0) e->dragon.dir = 3;
		    if(e->dragon.ya < 0) e->dragon.dir = 1;
		    else if(e->dragon.ya > 0) e->dragon.dir = 0;
		    
		    //if (e->dragon.randWalkTime > 0) {
			//    e->dragon.randWalkTime--;
			//    if (e->dragon.randWalkTime == 0) {
                if(nearestPlayer!=NULL) {
				    int xd = nearestPlayer->entity.x - e->x;
				    int yd = nearestPlayer->entity.y - e->y;
				    if (rand()%12 == 0 && xd * xd + yd * yd < 50 * 50) {
					    if (e->dragon.attackDelay == 0 && e->dragon.attackTime == 0) e->dragon.attackDelay = 40;
				    }
                }
			//    }
		    //}
			
			return;
		case ENTITY_DRAGONPROJECTILE: 
            e->dragonFire.age++;
		    if (e->dragonFire.age >= 30) {
			    removeEntityFromList(e, e->level, &eManager);
			    return;
		    }
		    e->dragonFire.xx += e->dragonFire.xa;
		    e->dragonFire.yy += e->dragonFire.ya;
		    e->x = (int) e->dragonFire.xx;
		    e->y = (int) e->dragonFire.yy;
		    
            if(nearestPlayer!=NULL && intersects(nearestPlayer->entity, e->x + e->dragonFire.xa - e->xr, e->y + e->dragonFire.ya - e->yr, e->x + e->dragonFire.xa + e->xr, e->y + e->dragonFire.ya + e->yr)){
                EntityVsEntity(e, &(nearestPlayer->entity));
			    removeEntityFromList(e, e->level, &eManager);
            }
            return;
		case ENTITY_ARROW: 
            e->arrow.age++;
		    if (e->arrow.age >= 260 || !move(e, e->arrow.xa, e->arrow.ya)) {
				//only drop arrows shot by player
				if(e->arrow.parent->type == ENTITY_PLAYER) addItemsToWorld(newItem(e->arrow.itemID,1), e->level, e->x+4, e->y+4, 1);
			    removeEntityFromList(e, e->level, &eManager);
			    return;
		    }
            return;
		case ENTITY_PASSIVE: 
            if (e->hurtTime > 0) e->hurtTime--;
            if (e->passive.randWalkTime == 0 && nearestPlayer!=NULL) {
			    int xd = nearestPlayer->entity.x - e->x;
			    int yd = nearestPlayer->entity.y - e->y;
				//flee from player
			    if (xd * xd + yd * yd < 40 * 40) {
				    e->passive.xa = 0;
				    e->passive.ya = 0;
				    if (xd < 0) e->passive.xa = +1; 
				    if (xd > 0) e->passive.xa = -1;
				    if (yd < 0) e->passive.ya = +1;
				    if (yd > 0) e->passive.ya = -1;
			    }
		    }
		    
		    if(e->passive.xa < 0) e->passive.dir = 2;
		    else if(e->passive.xa > 0) e->passive.dir = 3;
		    if(e->passive.ya < 0) e->passive.dir = 1;
		    else if(e->passive.ya > 0) e->passive.dir = 0;
		    
		    if(e->passive.xa != 0 || e->passive.ya != 0) e->passive.walkDist++;

		    int pspeed = syncTickCount & 1;
		    if (!moveMob(e, e->passive.xa * pspeed, e->passive.ya * pspeed) || (rand()%100) == 0) {
			    e->passive.randWalkTime = 60;
			    e->passive.xa = ((rand()%3) - 1) * (rand()%2);
			    e->passive.ya = ((rand()%3) - 1) * (rand()%2);
		    }
		    if (e->passive.randWalkTime > 0) e->passive.randWalkTime--;
            return;
		case ENTITY_GLOWWORM:
			if(worldData.daytime>5000 && worldData.daytime<6000) {
				if(rand()%200==0) {
					removeEntityFromList(e, e->level, &eManager);
					return;
				}
			} else if(worldData.daytime>6000 && worldData.daytime<18000) {
				removeEntityFromList(e, e->level, &eManager);
				return;
			}
			
		    int gspeed = (((syncTickCount & 0x3) == 3) ? 1 : 0);
		    if (!moveMob(e, e->glowworm.xa * gspeed, e->glowworm.ya * gspeed) || (e->glowworm.randWalkTime==0) || (rand()%20) == 0) {
				if(e->glowworm.randWalkTime != 0) {
					e->glowworm.waitTime = 20 + (rand()%60);
				}
				if(e->glowworm.waitTime == 0 || getTile(e->level, (e->x)>>4, (e->y)>>4)!=TILE_TREE) {
					e->glowworm.randWalkTime = 20;
					e->glowworm.xa = ((rand()%3) - 1) * (rand()%2);
					e->glowworm.ya = ((rand()%3) - 1) * (rand()%2);
				} else {
					e->glowworm.xa = 0;
					e->glowworm.ya = 0;
				}
		    }
		    if (e->glowworm.randWalkTime > 0) {
				e->glowworm.randWalkTime--;
				if(e->glowworm.randWalkTime==0 && (e->glowworm.xa != 0 || e->glowworm.xa != 0)) {
					e->glowworm.waitTime = 120 + (rand()%60);
				}
			} else if (e->glowworm.waitTime > 0) {
				e->glowworm.waitTime--;
			}
			return;
        case ENTITY_TEXTPARTICLE: tickEntityTextParticle(e); return;
        case ENTITY_SMASHPARTICLE: 
            ++e->smashParticle.age;
            if(e->smashParticle.age > 10) removeEntityFromList(e, e->level, &eManager);
        return;
    }
}

void trySpawn(int count, int level) {
    int i, j;
	for (i = 0; i < count; i++) {
        if(eManager.lastSlot[level] > 900) continue;
		Entity e;
		
		int minLevel = 1;
		int maxLevel = 1;
		if (level > 0) maxLevel = level;
		else if (level == 0) {
			minLevel = maxLevel = 4;
		}
		if(level > 4) {
			minLevel = maxLevel = 4;
		}
		
        int rx = rand()%128;
        int ry = rand()%128;
        int ex = (rx<<4)+8;
        int ey = (ry<<4)+8;
        
        //do not spawn near players
        for(j = 0; j<playerCount; j++) {
            if(level == players[j].entity.level && (ex > players[j].entity.x-160 && ey > players[j].entity.y-125 && ex < players[j].entity.x+160 && ey < players[j].entity.y+125)) continue;
        }
        
        //spawn if tile is free
		if (!tileIsSolid(worldData.map[level][rx+ry*128],&e)) {
			if(level==1 && (rand()%2)==0) { //passive entities on overworld
				e = newPassiveEntity(rand()%3, ex, ey, level);
			} else {
				int lvl = (rand()%(maxLevel - minLevel + 1)) + minLevel;
				int randMax = 1;
				
				if(level>1 || level==0) randMax = 2;
				if(level>3) randMax = 3;
				
				switch (rand()%(randMax+1)) {
				case 0:
					e = newSlimeEntity(lvl, ex, ey, level);
					break;
				case 1:
					e = newZombieEntity(lvl, ex, ey, level);
					break;
				case 2:
					e = newSkeletonEntity(lvl, ex, ey, level);
					break;
				case 3:
					e = newKnightEntity(lvl, ex, ey, level);
					break;
				}
			}
			addEntityToList(e, &eManager);
		}
	}
}

int getTile(s8 level, int x, int y){
    if(x < 0 || y < 0 || x > 128 || y > 128) return -1;
    return worldData.map[level][x+y*128];
}

void setTile(int id, s8 level, int x, int y){
    if(x < 0 || y < 0 || x > 128 || y > 128) return;
    worldData.map[level][x+y*128] = id;
	worldData.data[level][x+y*128] = 0; //reset data(set again if needed, hopefully this breaks nothing)
    
    sf2d_set_pixel(minimap[level], x, y, getMinimapColor(getLocalPlayer(), level, x, y));
}

int getData(s8 level, int x, int y){
    if(x < 0 || y < 0 || x > 128 || y > 128) return -1;
    return worldData.data[level][x+y*128];
}

void setData(int id, s8 level, int x, int y){
    if(x < 0 || y < 0 || x > 128 || y > 128) return;
    worldData.data[level][x+y*128] = id;
}

void addSmashParticles(s8 level, int x, int y, int damage) {
    char hurtText[11];
    sprintf(hurtText, "%d", damage);
    addEntityToList(newTextParticleEntity(hurtText, 0xFF0000FF, x, y, level), &eManager);
    addEntityToList(newSmashParticleEntity(x, y, level), &eManager);
}

void damageAndBreakTile(s8 level, int xt, int yt, int damage, int maxDamage, int replaceTile, int numItems, ...) {
    int i;
    
    //damage indicator
    addSmashParticles(level, xt<<4, yt<<4, damage);
    
    //do damage
    setData(getData(level, xt, yt)+damage, level, xt, yt);

    //tile has been destroyed
    if(getData(level, xt, yt)>maxDamage) {
        setTile(replaceTile, level, xt, yt);
        
        //drop items
        va_list al;
        numItems<<=1; //each item is the item+count, moved up here to get rid of warning
        va_start(al, numItems);
        numItems>>=1;
        for(i=0; i<numItems; ++i) {
            addItemsToWorld(va_arg(al,Item), level, (xt<<4)+8, (yt<<4)+8, va_arg(al, int));
        }
        va_end(al);
    }
}

void playerHurtTile(PlayerData *pd, int tile, s8 level, int xt, int yt, int damage, int dir){
    if(TESTGODMODE) damage = 99;
    
    //TODO: Most of this can be combined a lot
    switch(tile){
        case TILE_TREE:
            if(rand()%120==0)addEntityToList(newItemEntity(newItem(ITEM_APPLE,1), (xt<<4)+8,(yt<<4)+8, level), &eManager);
            damageAndBreakTile(level, xt, yt, damage, 20, TILE_GRASS, 2, newItem(ITEM_WOOD,1), rand()%2+1, newItem(ITEM_ACORN,1), rand()%2);
        break;
        case TILE_CACTUS:
            damageAndBreakTile(level, xt, yt, damage, 10, TILE_SAND, 1, newItem(ITEM_CACTUS,1), rand()%2+1);
        break;
        case TILE_ROCK:
            damageAndBreakTile(level, xt, yt, damage, 50, TILE_DIRT, 2, newItem(ITEM_STONE,1), rand()%4+1, newItem(ITEM_COAL,1), rand()%2);
        break;
        case TILE_HARDROCK:
            if((pd->activeItem->id != TOOL_PICKAXE || pd->activeItem->countLevel < 4) && !TESTGODMODE) damage = 0;
            damageAndBreakTile(level, xt, yt, damage, 200, TILE_DIRT, 2, newItem(ITEM_STONE,1), rand()%4+1, newItem(ITEM_COAL,1), rand()%2);
        break;
        case TILE_IRONORE:
        case TILE_GOLDORE:
        case TILE_GEMORE:
            if(pd->activeItem->id != TOOL_PICKAXE) damage = 0;
            addSmashParticles(level, xt<<4, yt<<4, damage);
            setData(getData(level, xt, yt)+damage, level, xt, yt);
            if(getData(level, xt, yt) > 0){
                 int count = rand() & 1;
			     if (getData(level, xt, yt) >= (rand()%10) + 3) {
					if(level!=5) setTile(TILE_DIRT, level, xt, yt);
					else setTile(TILE_DUNGEON_FLOOR, level, xt, yt);
				    count += 2;
			     }
                 if(tile==TILE_IRONORE) addItemsToWorld(newItem(ITEM_IRONORE,1), level, (xt<<4)+8, (yt<<4)+8, count);
                 if(tile==TILE_GOLDORE) addItemsToWorld(newItem(ITEM_GOLDORE,1), level, (xt<<4)+8, (yt<<4)+8, count);
                 if(tile==TILE_GEMORE) addItemsToWorld(newItem(ITEM_GEM,1), level, (xt<<4)+8, (yt<<4)+8, count);
            } break;
        case TILE_CLOUDCACTUS:
            if(pd->activeItem->id != TOOL_PICKAXE) damage = 0;
            addSmashParticles(level, xt<<4, yt<<4, damage);
            setData(getData(level, xt, yt)+damage, level, xt, yt);
            if(getData(level, xt, yt) > 0){
                 int count = rand() % 3;
			     if (getData(level, xt, yt) >= (rand()%10) + 3) {
				    setTile(TILE_CLOUD, level, xt, yt);
				    count += 3;
			     }
                 addItemsToWorld(newItem(ITEM_CLOUD,1), level, (xt<<4)+8, (yt<<4)+8, count);
            } break;
        case TILE_FARM:
			setTile(TILE_DIRT, level, xt, yt);
            break;
        case TILE_SAPLING_TREE:
			setTile(TILE_GRASS, level, xt, yt);
            break;
        case TILE_SAPLING_CACTUS:
			setTile(TILE_SAND, level, xt, yt);
            break;
        case TILE_WHEAT:
            if(getData(level, xt, yt) > -1){
                int age = getData(level, xt, yt);
                int count = (rand() % 2);
                if(age >= 80) count = (rand()%2) + 1;
                addItemsToWorld(newItem(ITEM_SEEDS,1), level, (xt<<4)+8, (yt<<4)+8, count);
                count = 0;
			    if(age == 100) count = (rand()%3) + 2;
                else if(age >= 80) count = (rand()%2) + 1;
                addItemsToWorld(newItem(ITEM_WHEAT,1), level, (xt<<4)+8, (yt<<4)+8, count);
				setTile(TILE_DIRT, level, xt, yt);
            } break;
        case TILE_FLOWER:
			setTile(TILE_GRASS, level, xt,yt);
            addItemsToWorld(newItem(ITEM_FLOWER,1), level, (xt<<4)+8, (yt<<4)+8, 1);
            break;
		case TILE_WOOD_WALL:
            damageAndBreakTile(level, xt, yt, damage, 20, TILE_DIRT, 1, newItem(ITEM_WALL_WOOD,1), 1);
			break;
		case TILE_STONE_WALL:
            damageAndBreakTile(level, xt, yt, damage, 30, TILE_DIRT, 1, newItem(ITEM_WALL_STONE,1), 1);
			break;
		case TILE_IRON_WALL:
            damageAndBreakTile(level, xt, yt, damage, 40, TILE_DIRT, 1, newItem(ITEM_WALL_IRON,1), 1);
			break;
		case TILE_GOLD_WALL:
            damageAndBreakTile(level, xt, yt, damage, 50, TILE_DIRT, 1, newItem(ITEM_WALL_GOLD,1), 1);
			break;
		case TILE_GEM_WALL:
            damageAndBreakTile(level, xt, yt, damage, 60, TILE_DIRT, 1, newItem(ITEM_WALL_GEM,1), 1);
			break;
        case TILE_BOOKSHELVES:
            addSmashParticles(level, xt<<4, yt<<4, damage);
            if(level!=5) setTile(TILE_DIRT, level, xt, yt);
            else setTile(TILE_DUNGEON_FLOOR, level, xt, yt);
            addItemsToWorld(newItem(ITEM_BOOKSHELVES,1), level, (xt<<4)+8, (yt<<4)+8, 1);
			break;
    }
    
}


void switchLevel(PlayerData *pd, s8 change){
    pd->entity.level+=change;
    if(pd->entity.level > 4) pd->entity.level = 0; else if(pd->entity.level < 0) pd->entity.level = 4;
    
    if(pd==getLocalPlayer()) {
        if(pd->entity.level == 1) sf2d_set_clear_color(0xFF6C6D82);
        else if(pd->entity.level > 1) sf2d_set_clear_color(0xFF666666);
        else sf2d_set_clear_color(0xFF007F00);
	}
    
	//for level 0 background
	updateLevel1Map();
}

void playerEntityInteract(PlayerData *pd, Entity* e){
    switch(e->type){
        case ENTITY_ITEM:
            if(e->entityItem.age > 30){
                addItemToInventory(e->entityItem.item,  &(pd->inventory));
                removeEntityFromList(e, e->level, &eManager);
                playSoundPositioned(snd_pickup, pd->entity.level, pd->entity.x, pd->entity.y);
                pd->score++;
            }
            break;
        case ENTITY_FURNITURE:
            switch(pd->entity.p.dir){
                case 0: if(pd->entity.y < e->y) move(e, 0,  2); break;
                case 1: if(pd->entity.y > e->y) move(e, 0, -2); break;
                case 2: if(pd->entity.x > e->x) move(e, -2, 0); break;
                case 3: if(pd->entity.x < e->x) move(e, 2,  0); break;
            }
            break;
    }
}

void entityTileInteract(Entity*e, int tile, s8 level, int x, int y){
    switch(tile){
        case TILE_STAIRS_DOWN: 
            if(e->type == ENTITY_PLAYER){
                switchLevel(e->p.data, 1); 
                e->x = (x << 4) + 8;
                e->y = (y << 4) + 8;
            }
			return;
        case TILE_STAIRS_UP: 
            if(e->type == ENTITY_PLAYER){
                switchLevel(e->p.data, -1); 
                e->x = (x << 4) + 8;
                e->y = (y << 4) + 8;
            }
			return;
        case TILE_CACTUS: if(e->type == ENTITY_PLAYER) hurtEntity(e ,1, -1, 0xFFAF00FF, NULL); return;
        case TILE_LAVA: if(e->type == ENTITY_PLAYER) hurtEntity(e, 1, -1, 0xFFAF00FF, NULL); return;
        case TILE_WHEAT: 
            if(e->type == ENTITY_PLAYER || e->type == ENTITY_ZOMBIE){
                if(getData(level, x, y) > -1 && rand()%20 == 0){
                    int age = getData(level, x, y);
                    int count = (rand() % 2);
                    if(age >= 80) count = (rand()%2) + 1;
                    addItemsToWorld(newItem(ITEM_SEEDS,1), level, (x<<4)+8, (y<<4)+8, count);
                    count = 0;
			        if(age == 100) count = (rand()%3) + 2;
                    else if(age >= 80) count = (rand()%2) + 1;
                    addItemsToWorld(newItem(ITEM_WHEAT,1), level, (x<<4)+8, (y<<4)+8, count);
				    setTile(TILE_DIRT, level, x, y);
                }
            }
			return;
        case TILE_FARM:
            if(e->type == ENTITY_PLAYER || e->type == ENTITY_ZOMBIE){
                if(rand()%20 == 0) setTile(TILE_DIRT, level, x, y);
            }
			return; 
		case TILE_SAND:
			if(e->type != ENTITY_ARROW && e->type != ENTITY_ITEM) {
				setData(10, level, x, y);
			}
			return;
		case TILE_DUNGEON_ENTRANCE:
			if(e->type == ENTITY_PLAYER) {
				e->p.data->ingameMenu = MENU_DUNGEON;
			}
			return;
    }
}

bool intersectsEntity(int x, int y, int r, Entity* e){
    return (x < e->x + e->xr && x + r > e->x && y < e->y + e->yr && y + r > e->y);
}

void openCraftingMenu(PlayerData *pd, RecipeManager *rm, char *title) {
    pd->currentCraftTitle = title;
    pd->ingameMenu = MENU_CRAFTING;
    
    cloneRecipeManager(rm, &(pd->currentRecipes));
    checkCanCraftRecipes(&(pd->currentRecipes), &(pd->inventory));
    sortRecipes(&(pd->currentRecipes));
}

bool useEntity(PlayerData *pd, Entity* e) {
    if(e->type == ENTITY_FURNITURE){
        switch(e->entityFurniture.itemID){
            case ITEM_WORKBENCH:
                openCraftingMenu(pd, &workbenchRecipes, "Crafting");
                return true;
            case ITEM_FURNACE:
                openCraftingMenu(pd, &furnaceRecipes, "Smelting");
                return true;
            case ITEM_OVEN:
                openCraftingMenu(pd, &ovenRecipes, "Cooking");
                return true;
            case ITEM_ANVIL:
                openCraftingMenu(pd, &anvilRecipes, "Smithing");
                return true;
            case ITEM_LOOM:
                openCraftingMenu(pd, &loomRecipes, "Crafting");
                return true;
			case ITEM_ENCHANTER:
                openCraftingMenu(pd, &enchanterRecipes, "Crafting");
                return true;
            case ITEM_CHEST:
                pd->curChestEntity = e;
                pd->ingameMenuInvSel = 0;
                pd->ingameMenuInvSelOther = 0;
                pd->curChestEntityR = 0;
                pd->ingameMenu = MENU_CONTAINER; 
                return true;
        }
    } else if(e->type == ENTITY_NPC) {
        openNPCMenu(pd, e->npc.type);
        return true;
    }
    return false;
}


bool isWater(s8 level, int xt, int yt){
    return getTile(level, xt, yt)==TILE_WATER;
}

bool dungeonActive() {
    //check if dungeon already exists (ie someone is in there)
    int i;
    for(i = 0; i < playerCount; i++) {
        if(players[i].entity.level==5) {
            return true;
        }
    }
    return false;
}

void enterDungeon(PlayerData *pd) {
    //create new one if needed
    if(!dungeonActive()) {
        //reset Entities
        (&eManager)->lastSlot[5] = 0;
        (&eManager)->entities[5][0] = nullEntity;
        
        //create map
        createAndValidateDungeonMap(128, 128, 5, worldData.map[5], worldData.data[5]);
        
        //reset minimap clear state
        int xd,yd;
        for(xd = 0; xd < 128; ++xd) {
            for(yd = 0; yd < 128; ++yd) {
                setMinimapVisible(pd, 5, xd, yd, false);
            }
        }
        initMinimapLevel(pd, 5);
        
        //spawn new entities
        trySpawn(500, 5);
    }
    
    
	pd->entity.level = 5;
    pd->entity.x = ((128/2) << 4) + 8;
	pd->entity.y = ((128/2) << 4) + 8;
}

void leaveDungeon(PlayerData *pd) {
	pd->entity.level = 4;
	pd->entity.x = ((128/2) << 4) + 8;
	pd->entity.y = ((128/2) << 4) + 8;
	
    //clear dungeon if empty
    if(!dungeonActive()) {
        //reset Entities
        (&eManager)->lastSlot[5] = 0;
        (&eManager)->entities[5][0] = nullEntity;
    }
}

void setMinimapVisible(PlayerData *pd, int level, int x, int y, bool visible) {
	if(visible) {
		pd->minimapData[x + y * 128] = pd->minimapData[x + y * 128] | (1 << level);
	} else {
		pd->minimapData[x + y * 128] = pd->minimapData[x + y * 128] & (0xFF - (1 << level));
	}
	
    if(pd==getLocalPlayer()) sf2d_set_pixel(minimap[level], x, y, getMinimapColor(pd, level, x, y));
}

bool getMinimapVisible(PlayerData *pd, int level, int x, int y) {
	return (pd->minimapData[x + y * 128] & (1 << level)) > 0;
}

u32 getMinimapColor(PlayerData *pd, int level, int x, int y) {
	if(getMinimapVisible(pd, level, x, y) || (pd->entity.level==0 && level==1)) return getTileColor(worldData.map[level][x + y * 128]);
	else return getTileColor(worldData.map[level][x + y * 128]) & 0xFFFFFF00;
}

void initMinimapLevel(PlayerData *pd, int level) {
	int x;
	int y;
    
    if(pd!=getLocalPlayer()) return;
	
	for (x = 0; x < 128; ++x) {
		for (y = 0; y < 128; ++y) {
			/* Minimaps */
			sf2d_set_pixel(minimap[level], x, y, getMinimapColor(pd, level, x, y));
		}
	}
}

void updateLevel1Map() {
    initMinimapLevel(getLocalPlayer(), 1);
}


void reloadColors() {
	dirtColor[0] = SWAP_UINT32(sf2d_get_pixel(icons, 16, 0)); 
	dirtColor[1] = SWAP_UINT32(sf2d_get_pixel(icons, 16, 1)); 
	dirtColor[2] = SWAP_UINT32(sf2d_get_pixel(icons, 16, 2)); 
	dirtColor[3] = SWAP_UINT32(sf2d_get_pixel(icons, 16, 3)); 
	dirtColor[4] = SWAP_UINT32(sf2d_get_pixel(icons, 16, 4)); 
	
	grassColor = SWAP_UINT32(sf2d_get_pixel(icons, 17, 0)); 
    myceliumColor = SWAP_UINT32(sf2d_get_pixel(icons, 17, 1)); 
    mushroomColor = SWAP_UINT32(sf2d_get_pixel(icons, 17, 2)); 

	sandColor = SWAP_UINT32(sf2d_get_pixel(icons, 18, 0)); 
	
	waterColor[0] = SWAP_UINT32(sf2d_get_pixel(icons, 19, 0)); 
	waterColor[1] = SWAP_UINT32(sf2d_get_pixel(icons, 19, 1)); 
	
	lavaColor[0] = SWAP_UINT32(sf2d_get_pixel(icons, 20, 0)); 
	lavaColor[1] = SWAP_UINT32(sf2d_get_pixel(icons, 20, 1)); 
	
	rockColor[0] = SWAP_UINT32(sf2d_get_pixel(icons, 21, 0)); 
	rockColor[1] = SWAP_UINT32(sf2d_get_pixel(icons, 21, 1)); 
	rockColor[2] = SWAP_UINT32(sf2d_get_pixel(icons, 21, 2)); 
	rockColor[3] = SWAP_UINT32(sf2d_get_pixel(icons, 21, 3)); 
	
	woodColor = SWAP_UINT32(sf2d_get_pixel(icons, 22, 0)); 
	
	ironColor = SWAP_UINT32(sf2d_get_pixel(icons, 23, 0)); 
	goldColor = SWAP_UINT32(sf2d_get_pixel(icons, 23, 1)); 
	gemColor = SWAP_UINT32(sf2d_get_pixel(icons, 23, 2)); 
	
	dungeonColor[0] = SWAP_UINT32(sf2d_get_pixel(icons, 24, 0)); 
	dungeonColor[1] = SWAP_UINT32(sf2d_get_pixel(icons, 24, 1)); 
    
    snowColor = SWAP_UINT32(sf2d_get_pixel(icons, 25, 0));
    iceColor = SWAP_UINT32(sf2d_get_pixel(icons, 25, 1));
}