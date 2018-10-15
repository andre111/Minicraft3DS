#include "IngameMenu.h"

#include "Data.h"
#include "Globals.h"
#include "Ingame.h"
#include "Player.h"
#include "SaveLoad.h"
#include "Render.h"
#include "network/Synchronizer.h"

char pOptions[][24] = {"Return to game", "Save Progress", "Exit to title"};

void ingameMenuTick(PlayerData *pd, int menu) {
    switch(menu) {
        case MENU_PAUSED:
            if(!pd->ingameMenuAreYouSure && !pd->ingameMenuAreYouSureSave){
                if (pd->ingameMenuTimer > 0) --pd->ingameMenuTimer;
                if (pd->inputs.k_pause.clicked || pd->inputs.k_decline.clicked) pd->ingameMenu = MENU_NONE;
                if (pd->inputs.k_up.clicked) { --pd->ingameMenuSelection; if(pd->ingameMenuSelection < 0) pd->ingameMenuSelection=2; }
                if (pd->inputs.k_down.clicked) { ++pd->ingameMenuSelection; if(pd->ingameMenuSelection > 2) pd->ingameMenuSelection=0; }
                if (pd->inputs.k_accept.clicked){
                    switch(pd->ingameMenuSelection){
                    case 0:      
                        pd->ingameMenu = MENU_NONE;
                        break;
                    case 1:
                        if(!dungeonActive()) pd->ingameMenuAreYouSureSave = true;
                        break;
                    case 2:
                        pd->ingameMenuAreYouSure = true;
                        break;
                    }
                }
            } else if(pd->ingameMenuAreYouSureSave) {
                if (pd->inputs.k_accept.clicked){
                    pd->ingameMenuTimer = 60;
                    
                    if(playerLocalID==0) {
                        saveWorld(currentFileName, &eManager, &worldData, players, playerCount);
                    }
                    pd->ingameMenuAreYouSureSave = false;
                    pd->ingameMenuAreYouSure = false;
                } else if (pd->inputs.k_decline.clicked){
                    pd->ingameMenuAreYouSureSave = false;
                    pd->ingameMenuAreYouSure = false;
                }
            } else if(pd->ingameMenuAreYouSure) {
                if (pd->inputs.k_accept.clicked){
                    pd->ingameMenuAreYouSure = false;
                    pd->ingameMenuAreYouSureSave = false;
                    
                    exitGame();
                } else if (pd->inputs.k_decline.clicked){
                    pd->ingameMenuAreYouSure = false;
                    pd->ingameMenuAreYouSureSave = false;
                }
            }
        break;
        case MENU_INVENTORY:
            if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked){
                pd->ingameMenu = MENU_NONE;
                pd->activeItem = &noItem;
                pd->entity.p.isCarrying = false;
            }
            //inventory
            if(pd->ingameMenuSelection==0) {
                if (pd->inputs.k_accept.clicked){ // Select item from inventory
                    if(pd->inventory.lastSlot!=0){
                        Item median = pd->inventory.items[pd->ingameMenuInvSel]; // create copy of item.
                        removeItemFromInventory(pd->ingameMenuInvSel, &(pd->inventory)); // remove original
                        pushItemToInventoryFront(median, &(pd->inventory)); // add copy to front
                        playerSetActiveItem(pd, &(pd->inventory.items[0])); // active item = copy.
                    }
                    pd->ingameMenu = MENU_NONE;
                }
                if (pd->inputs.k_up.clicked) { --pd->ingameMenuInvSel; if(pd->ingameMenuInvSel < 0)pd->ingameMenuInvSel=pd->inventory.lastSlot-1; }
                if (pd->inputs.k_down.clicked) { ++pd->ingameMenuInvSel; if(pd->ingameMenuInvSel > pd->inventory.lastSlot-1)pd->ingameMenuInvSel=0; }
                
                if (pd->inputs.k_right.clicked) { pd->ingameMenuSelection = 1; }
            //player stats
            } else if(pd->ingameMenuSelection==1) {
                
                if (pd->inputs.k_left.clicked) { pd->ingameMenuSelection = 0; }
            }
        break;
        
        case MENU_CRAFTING:
            if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked) pd->ingameMenu = MENU_NONE;
            if (pd->inputs.k_accept.clicked){
                if(craftItem(&(pd->currentRecipes), &(pd->currentRecipes.recipes[pd->ingameMenuInvSel]), &(pd->inventory))){
                    playSoundPositioned(snd_craft, pd->entity.level, pd->entity.x, pd->entity.y);
                    //reset active item pointer, because it could posibly point to garbage now
                    pd->activeItem = &noItem;
                }
            }
            if (pd->inputs.k_up.clicked) { --pd->ingameMenuInvSel; if(pd->ingameMenuInvSel < 0)pd->ingameMenuInvSel=pd->currentRecipes.size-1; }
            if (pd->inputs.k_down.clicked) { ++pd->ingameMenuInvSel; if(pd->ingameMenuInvSel > pd->currentRecipes.size-1)pd->ingameMenuInvSel=0; }
        break;
        
        case MENU_WIN:
            if (pd->inputs.k_accept.clicked){ 
                pd->ingameMenu = MENU_NONE;
                pd->entity.p.hasWon = false;
            }
        break;
        case MENU_LOSE:
            if (pd->inputs.k_accept.clicked){ 
                pd->ingameMenu = MENU_NONE;
                pd->entity.p.isDead = false;
                pd->entity.p.health = 10;
                pd->entity.level = 1;
                playerSpawn(pd);
                //TODO: This canceled to main menu, but what should I do in multiplayer?
            }
            pd->entity.hurtTime = 10;
        break;
        
        case MENU_CONTAINER:
            if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked) pd->ingameMenu = MENU_NONE;
            
            if (pd->inputs.k_left.clicked) {
                pd->curChestEntityR = 0;
                int tmp = pd->ingameMenuInvSel;
                pd->ingameMenuInvSel = pd->ingameMenuInvSelOther;
                pd->ingameMenuInvSelOther = tmp;
            }
            if (pd->inputs.k_right.clicked) {
                pd->curChestEntityR = 1;
                int tmp = pd->ingameMenuInvSel;
                pd->ingameMenuInvSel = pd->ingameMenuInvSelOther;
                pd->ingameMenuInvSelOther = tmp;
            }
            
            Inventory* i1 = pd->curChestEntityR == 1 ? &(pd->inventory) : pd->curChestEntity->entityFurniture.inv;
            Inventory* i2 = pd->curChestEntityR == 0 ? &(pd->inventory) : pd->curChestEntity->entityFurniture.inv;
            int len = i1->lastSlot;
            if (pd->ingameMenuInvSel < 0) pd->ingameMenuInvSel = 0;
            if (pd->ingameMenuInvSel >= len) pd->ingameMenuInvSel = len - 1;
            if (pd->inputs.k_up.clicked) --pd->ingameMenuInvSel;
            if (pd->inputs.k_down.clicked) ++pd->ingameMenuInvSel;
            if (len == 0) pd->ingameMenuInvSel = 0;
            if (pd->ingameMenuInvSel < 0) pd->ingameMenuInvSel += len;
            if (pd->ingameMenuInvSel >= len) pd->ingameMenuInvSel -= len;
            
            if(pd->inputs.k_accept.clicked && len > 0){
                Item* pullItem = &i1->items[pd->ingameMenuInvSel];
                Item pushItem = newItem(pullItem->id, pullItem->countLevel);
                pushItem.chestPtr = pullItem->chestPtr;
                pushItemToInventoryFront(pushItem, i2);
                if(i2 == &(pd->inventory)){
                    int newslot = pd->activeItem->slotNum + 1;
                    pd->activeItem = &(pd->inventory.items[newslot]);
                } else if(pullItem == pd->activeItem){
                    pd->activeItem = &noItem;
                }
                removeItemFromCurrentInv(pullItem);
                if (pd->ingameMenuInvSel >= i1->lastSlot) pd->ingameMenuInvSel = i1->lastSlot - 1;
            }
        break;
        
        case MENU_DUNGEON:
            if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked) pd->ingameMenu = MENU_NONE;
            
            if(pd->inputs.k_accept.clicked) {
                if(pd->entity.level!=5) {
                    Item * item = getItemFromInventory(ITEM_DUNGEON_KEY, &(pd->inventory));
                    if(item!=NULL) {
                        --item->countLevel;
                        if(item->countLevel==0) {
                            removeItemFromCurrentInv(item);
                        }
                        
                        enterDungeon(pd);
                    } else if(TESTGODMODE) {
                        enterDungeon(pd);
                    }
                } else {
                    leaveDungeon(pd);
                }
                
                pd->ingameMenu = MENU_NONE;
            }
        break;
        
        case MENU_NPC:
            tickNPCMenu(pd);
        break;
        
        case MENU_CHARACTER_CUSTOMIZE:
            if (pd->inputs.k_up.clicked) { --pd->ingameMenuSelection; if(pd->ingameMenuSelection < 0) pd->ingameMenuSelection=7; }
            if (pd->inputs.k_down.clicked) { ++pd->ingameMenuSelection; if(pd->ingameMenuSelection > 7) pd->ingameMenuSelection=0; }
            
            u8 wrap = 0;
            wrap = wrap - 1;
            
            pd->entity.p.health = 10;
            pd->entity.hurtTime = 10;
            
            //head
            if(pd->ingameMenuSelection==0) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.head; if(pd->sprite.head == wrap) pd->sprite.head=PLAYER_SPRITE_HEAD_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.head; if(pd->sprite.head > PLAYER_SPRITE_HEAD_COUNT-1) pd->sprite.head=0; }
            //eyes
            } else if(pd->ingameMenuSelection==1) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.eyes; if(pd->sprite.eyes == wrap) pd->sprite.eyes=PLAYER_SPRITE_EYES_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.eyes; if(pd->sprite.eyes > PLAYER_SPRITE_EYES_COUNT-1) pd->sprite.eyes=0; }
            //body
            } else if(pd->ingameMenuSelection==2) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.body; if(pd->sprite.body == wrap) pd->sprite.body=PLAYER_SPRITE_BODY_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.body; if(pd->sprite.body > PLAYER_SPRITE_BODY_COUNT-1) pd->sprite.body=0; }
            //arms
            } else if(pd->ingameMenuSelection==3) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.arms; if(pd->sprite.arms == wrap) pd->sprite.arms=PLAYER_SPRITE_ARMS_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.arms; if(pd->sprite.arms > PLAYER_SPRITE_ARMS_COUNT-1) pd->sprite.arms=0; }
            //legs
            } else if(pd->ingameMenuSelection==4) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.legs; if(pd->sprite.legs == wrap) pd->sprite.legs=PLAYER_SPRITE_LEGS_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.legs; if(pd->sprite.legs > PLAYER_SPRITE_LEGS_COUNT-1) pd->sprite.legs=0; }
            //accs
            } else if(pd->ingameMenuSelection==5) {
                if (pd->inputs.k_left.clicked) { --pd->sprite.accs; if(pd->sprite.accs == wrap) pd->sprite.accs=PLAYER_SPRITE_ACCS_COUNT-1; }
                if (pd->inputs.k_right.clicked) { ++pd->sprite.accs; if(pd->sprite.accs > PLAYER_SPRITE_ACCS_COUNT-1) pd->sprite.accs=0; }
            //rotation
            } else if(pd->ingameMenuSelection==6) {
                if (pd->inputs.k_left.clicked) { --pd->entity.p.dir; if(pd->entity.p.dir == wrap) pd->entity.p.dir=3; }
                if (pd->inputs.k_right.clicked) { ++pd->entity.p.dir; if(pd->entity.p.dir > 3) pd->entity.p.dir=0; }
            //done
            } else if(pd->ingameMenuSelection==7) {
                if(pd->inputs.k_accept.clicked) {
                    //TODO: are you sure dialog?
                    pd->ingameMenu = 0;
                    pd->ingameMenuSelection = 0;
                    pd->sprite.choosen = true;
                }
            }
        break;
    }
}


u8 opacity = 255;
bool rev = true;
char scoreText[15];

void ingameMenuRender(PlayerData *pd, int menu) {
    int i;
    int oox, ooy, osx;
    
    switch(menu) {
        case MENU_PAUSED:
            renderFrame(1,1,24,14,0xFF1010AF);
            renderText("Paused",164,32);
            for(i = 3; i >= 0; --i){
                char* msg = pOptions[i];
                u32 color = 0xFF7F7F7F;
                if(i == pd->ingameMenuSelection) color = 0xFFFFFFFF;
                if((i == 1 && dungeonActive())) {
                    color = 0xFF7F7FFF;
                    if(i == pd->ingameMenuSelection) color = 0xFFAFAFFF;
                }
                renderTextColor(msg,(400 - (strlen(msg) * 12))/2, (i * 24) + 88, color);    
            }
            
            if(pd->ingameMenuTimer > 0) renderTextColor("Game Saved!", (400-(11*12))/2, 64, 0xFF20FF20);
            
            if(pd->ingameMenuAreYouSure || pd->ingameMenuAreYouSureSave){
                if(pd->ingameMenuAreYouSure) renderFrame(6,5,19,10,0xFF10108F);
                else renderFrame(6,5,19,10,0xFF108F10);
                
                renderText("Are you sure?",122,96);
                renderText("   Yes", 164, 117);
                renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 166, 114, 1);
                renderText("   No", 170, 133);
                renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 166, 130, 1);
            }
        break;  
        case MENU_WIN:
            renderFrame(5,3,21,12,0xFFFF1010);
            if(!rev){ opacity+=5; if(opacity == 255) rev = true; }
            else { opacity-=5; if(opacity == 100) rev = false; }
            sprintf(scoreText,"Score: %d", pd->score);
            renderTextColor("You Win!",158,76,0x0000AFAF + (opacity << 24));
            renderText(scoreText, 200-((strlen(scoreText)-1)*6), 100);
            renderText("Press   to continue", 96, 150);
            renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 166, 148, 1);
        break;  
        case MENU_LOSE:
            renderFrame(5,3,21,12,0xFFFF1010);
            if(!rev){ opacity+=5; if(opacity == 255) rev = true; }
            else { opacity-=5; if(opacity == 100) rev = false; }
            sprintf(scoreText,"Score: %d", pd->score);
            renderTextColor("You DIED!",158,76,0x000000AF + (opacity << 24));
            renderText(scoreText, 200-((strlen(scoreText)-1)*6), 100);
            renderText("Press   to continue", 96, 150);
            renderButtonIcon(localInputs.k_attack.input & -localInputs.k_attack.input, 166, 148, 1);
        break;  
        case MENU_INVENTORY:
            if(pd->ingameMenuSelection == 1) { offsetX = 176; offsetY = 0;}
            else {offsetX = 0; offsetY = 0;}
        
            //inventory
            renderFrame(1, 1, 23, 14, 0xFFFF1010);
            renderTextColor("Inventory", 24+1, 14+1, 0xFF000000);
            renderTextColor("Inventory", 24, 14, 0xFF6FE2E2);
            renderItemList(&(pd->inventory), 1, 1, 23, 14, pd->ingameMenuInvSel);
            
            //player stats
            renderFrame(24, 1, 46, 14, 0xFFFF1010);
            renderTextColor("Character", 368+24+1, 14+1, 0xFF000000);
            renderTextColor("Character", 368+24, 14, 0xFF6FE2E2);
            
            //player sprite (TODO: Mostly Duplicated in MENU_CHARACTER_CUSTOMIZE, move to function?)
            renderFrame(25, 2, 34, 10, 0xFF909090);
            oox = offsetX;
            ooy = offsetY;
            osx = playerScale;
            //move player sprite to 0/0
            offsetX = offsetX + pd->entity.x - 8;
            offsetY = offsetY + pd->entity.y - 8;
            //move to where I want it
            offsetX -= 212;
            offsetY -= 24;
            playerScale = 6;
            renderPlayer(pd);
            offsetX = oox;
            offsetY = ooy;
            playerScale = osx;
            
            //TODO: armor
            
            //effects
            renderTextColor("Effects", 360+208+1, 34+1, 0xFF000000);
            renderTextColor("Effects", 360+208, 34, 0xFFFFFFFF);
            int epos = 0;
            for(i = 0; i < EFFECTS_MAX; i++) {
                if(playerEffectActive(pd, i)) {
                    //can only display a limited amount of effects at once
                    if(epos==3) {
                        renderTextColor("...", 362+208, 34+24+epos*48, 0xFFFFFFFF);
                        break;
                    }
                    
                    //draw information
                    renderFrame(35, 3+epos*3, 45, 3+epos*3+3, 0xFFFF1010);
                    
                    u32 etime = playerEffectGetTime(pd, i);
                    if(etime!=EFFECTS_DURATION_INFINITE) {
                        char etimest[20];
                        sprintf(etimest, "       %02lu:%02lu", (etime/60/60)%60, (etime/60)%60);
                        
                        renderTextColor(etimest, 362+208, 34+24+epos*48, 0xFFFFFFFF);
                    }
                    render((362+208)>>1, (34+23+epos*48)>>1, effectGetIconX(i), effectGetIconY(i), 0);
                    renderTextColor(effectGetName(i), 362+208, 34+40+epos*48, 0xFFFFFFFF);
                    
                    epos++;
                }
            }
            
            
            offsetX = 0; offsetY = 0;
        break;  
        case MENU_CRAFTING:
            renderFrame(15,1,24,4,0xFFFF1010);
            renderTextColor("Have",248+1,14+1,0xFF000000);
            renderTextColor("Have",248,14,0xFF6FE2E2);
            renderFrame(15,5,24,14,0xFFFF1010);
            renderTextColor("Cost",248+1,78+1,0xFF000000);
            renderTextColor("Cost",248,78,0xFF6FE2E2);
            renderFrame(1,1,14,14,0xFFFF1010);
            renderTextColor(pd->currentCraftTitle,24+1,14+1,0xFF000000);
            renderTextColor(pd->currentCraftTitle,24,14,0xFF6FE2E2);
            renderRecipes(&(pd->currentRecipes), 1, 1, 14, 14, pd->ingameMenuInvSel);
            
            Recipe* rec = &(pd->currentRecipes.recipes[pd->ingameMenuInvSel]);
            renderItemIcon(rec->itemResult,rec->itemAmountLevel,128,16);
            char craftText[12];
            sprintf(craftText, "%d", countItemInv(rec->itemResult, rec->itemAmountLevel, &(pd->inventory)));
            renderText(craftText,274,34);
            
            if(rec->numOfCosts > 0){
                int i;
                for(i = 0; i < rec->numOfCosts; i++){
                    int amnt = countItemInv(rec->costs[i].costItem,0, &(pd->inventory));
                    int ttlCst = rec->costs[i].costAmount;
                    int col = 0xFFFFFFFF; if(amnt<ttlCst) col = 0xFF7F7F7F;
                    renderItemIcon(rec->costs[i].costItem,1,128,48+(i*8));
                    sprintf(craftText,"%d/%d",amnt,ttlCst);
                    renderTextColor(craftText,274,96+(i*18),col);
                }
            }
        break;  
        
        case MENU_CONTAINER:
            if (pd->curChestEntityR == 1) { offsetX = 48; offsetY = 0;}
            else {offsetX = 0; offsetY = 0;}
            
            renderFrame(1,1,15,14,0xFFFF1010);
            renderTextColor("Chest",24+1,14+1,0xFF000000);
            renderTextColor("Chest",24,14,0xFF6FE2E2);
            renderItemList(pd->curChestEntity->entityFurniture.inv,1,1,15,14,
            pd->curChestEntityR == 0 ? pd->ingameMenuInvSel : -pd->ingameMenuInvSelOther - 1);
            renderFrame(16,1,30,14,0xFFFF1010);
            renderTextColor("Inventory",264+1,14+1,0xFF000000);
            renderTextColor("Inventory",264,14,0xFF6FE2E2);
            renderItemList(&(pd->inventory),16,1,30,14,
            pd->curChestEntityR == 1 ? pd->ingameMenuInvSel : -pd->ingameMenuInvSelOther - 1);
            offsetX = 0;offsetY = 0;
        break;
        
        case MENU_DUNGEON: 
            renderFrame(1,1,24,14,0xFFFF1010);
            if(pd->entity.level!=5) {
                renderTextColor("Dungeon Entrance",24+1,14+1,0xFF000000);
                renderTextColor("Dungeon Entrance",24,14,0xFF6FE2E2);
                
                renderText("Warning: ", 32, 32);
                renderText("You need a Dungeon Key to   ", 32, 56);
                renderText("enter and cannot save while ", 32, 72);
                renderText("being in the Dungeon!       ", 32, 88);
                renderText("After leaving you will need ", 32, 112);
                renderText("a new Dungeon Key for       ", 32, 128);
                renderText("entering another Dungeon!   ", 32, 144);
                
                renderText("   Enter", 148, 171);
            } else {
                renderTextColor("Dungeon Exit",24+1,14+1,0xFF000000);
                renderTextColor("Dungeon Exit",24,14,0xFF6FE2E2);
                
                renderText("Warning: ", 32, 32);
                renderText("The Dungeon and everything  ", 32, 56);
                renderText("in it will disappear when   ", 32, 72);
                renderText("you leave it!               ", 32, 88);
                renderText("You will need a new Dungeon ", 32, 112);
                renderText("Key for entering another    ", 32, 128);
                renderText("Dungeon again!              ", 32, 144);
                
                renderText("   Leave", 148, 171);
            }
            
            renderButtonIcon(localInputs.k_accept.input & -localInputs.k_accept.input, 150, 168, 1);
            renderText("   Stay", 148, 195);
            renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 150, 192, 1);
        break;
        
        case MENU_NPC:
            renderNPCMenu(&(pd->npcMenuData));
        break;
        
        case MENU_CHARACTER_CUSTOMIZE:
            renderFrame(1,1,24,14,0xFFFF1010);
            renderTextColor("Character",24+1,14+1,0xFF000000);
            renderTextColor("Character",24,14,0xFF6FE2E2);
            
            renderText("Head: ", 32, 52);
            renderText("Eyes: ", 32, 68);
            renderText("Body: ", 32, 84);
            renderText("Arms: ", 32, 100);
            renderText("Legs: ", 32, 116);
            renderText("Accs: ", 32, 132);
            
            renderText("Rot.: ", 32, 158);
            
            //for the dynamic part
            char display[30];
            
            sprintf(display, pd->ingameMenuSelection==0 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.head+1, PLAYER_SPRITE_HEAD_COUNT);
            renderText(display, 96, 52);
            sprintf(display, pd->ingameMenuSelection==1 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.eyes+1, PLAYER_SPRITE_EYES_COUNT);
            renderText(display, 96, 68);
            sprintf(display, pd->ingameMenuSelection==2 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.body+1, PLAYER_SPRITE_BODY_COUNT);
            renderText(display, 96, 84);
            sprintf(display, pd->ingameMenuSelection==3 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.arms+1, PLAYER_SPRITE_ARMS_COUNT);
            renderText(display, 96, 100);
            sprintf(display, pd->ingameMenuSelection==4 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.legs+1, PLAYER_SPRITE_LEGS_COUNT);
            renderText(display, 96, 116);
            sprintf(display, pd->ingameMenuSelection==5 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->sprite.accs+1, PLAYER_SPRITE_ACCS_COUNT);
            renderText(display, 96, 132);
            sprintf(display, pd->ingameMenuSelection==6 ? "< %02i/%02i >" : "  %02i/%02i  ", pd->entity.p.dir+1, 4);
            renderText(display, 96, 158);
            sprintf(display, pd->ingameMenuSelection==7 ? "< Done  >" : "  Done   ");
            renderText(display, 96, 174);
            
            //player sprite
            renderFrame(13,3,22,12,0xFF909090);
            oox = offsetX;
            ooy = offsetY;
            osx = playerScale;
            //move player sprite to 0/0
            offsetX = pd->entity.x - 8;
            offsetY = pd->entity.y - 8;
            //move to where I want it
            offsetX -= 108;
            offsetY -= 28;
            playerScale = 8;
            renderPlayer(pd);
            
            offsetX = oox;
            offsetY = ooy;
            playerScale = osx;
        break;
    }
}


//touch menu
void tickTouchMap(PlayerData *pd){
    if(pd->mapShouldRender){
        if(pd->inputs.k_touch.px > 0 || pd->inputs.k_touch.py > 0){
            // Plus/Minus zoom button
            if(pd->inputs.k_touch.py > 204 && pd->inputs.k_touch.py < 232){
                if(pd->inputs.k_touch.px > 284 && pd->inputs.k_touch.px < 312){
                    if(pd->mapZoomLevel > 4) return;
                    if(!pd->touchIsChangingSize && !pd->touchIsDraggingMap){
                        pd->mapZoomLevel += 2;
                        pd->mapScrollX -= (50 * (pd->mapZoomLevel/2));
                        pd->mapScrollY -= (40 * (pd->mapZoomLevel/2));
                        pd->touchIsChangingSize = true;
                        sprintf(pd->mapText, "x%d", pd->mapZoomLevel);
                    }
                    if(pd->mapScrollX < 320-(128*pd->mapZoomLevel)) pd->mapScrollX = 320-(128*pd->mapZoomLevel);
                    else if(pd->mapScrollX > 0) pd->mapScrollX = 0;
                    if(pd->mapScrollY < 240-(128*pd->mapZoomLevel)) pd->mapScrollY = 240-(128*pd->mapZoomLevel);
                    else if(pd->mapScrollY > 0) pd->mapScrollY = 0;
                    return;
                } else if(pd->inputs.k_touch.px > 256 && pd->inputs.k_touch.px < 284){
                    if(pd->mapZoomLevel < 4) return;
                    if(!pd->touchIsChangingSize && !pd->touchIsDraggingMap){
                        pd->mapScrollX += (50 * (pd->mapZoomLevel/2));
                        pd->mapScrollY += (40 * (pd->mapZoomLevel/2));
                        pd->mapZoomLevel -= 2;
                        pd->touchIsChangingSize = true;
                        sprintf(pd->mapText, "x%d", pd->mapZoomLevel);
                    }
                    if(pd->mapScrollX < 320-(128*pd->mapZoomLevel)) pd->mapScrollX = 320-(128*pd->mapZoomLevel);
                    else if(pd->mapScrollX > 0) pd->mapScrollX = 0;
                    if(pd->mapScrollY < 240-(128*pd->mapZoomLevel)) pd->mapScrollY = 240-(128*pd->mapZoomLevel);
                    else if(pd->mapScrollY > 0) pd->mapScrollY = 0;
                    return;
                }
            } else if(pd->inputs.k_touch.py > 8 && pd->inputs.k_touch.py < 40 && pd->inputs.k_touch.px > 284 && pd->inputs.k_touch.px < 312){
                // Exit Button
                if(!pd->touchIsChangingSize && !pd->touchIsDraggingMap){
                    pd->mapShouldRender = false;
                    return;
                }
            }
        
            if(!pd->touchIsDraggingMap){
                pd->touchLastX = pd->inputs.k_touch.px;
                pd->touchLastY = pd->inputs.k_touch.py;    
            }
            if(pd->mapZoomLevel > 2){
                int dx = pd->touchLastX - pd->inputs.k_touch.px;
                if(dx > 1 || dx < -1){
                    pd->mapScrollX -= dx;
                    if(pd->mapScrollX < 320-(128*pd->mapZoomLevel)) pd->mapScrollX = 320-(128*pd->mapZoomLevel);
                    else if(pd->mapScrollX > 0) pd->mapScrollX = 0;
                }
                pd->touchLastX = pd->inputs.k_touch.px;
            }
        
            int dy = pd->touchLastY - pd->inputs.k_touch.py;
            if(dy > 1 || dy < -1){
                pd->mapScrollY -= dy;
                if(pd->mapScrollY < 240-(128*pd->mapZoomLevel)) pd->mapScrollY = 240-(128*pd->mapZoomLevel);
                else if(pd->mapScrollY > 0) pd->mapScrollY = 0;
            }
            pd->touchLastY = pd->inputs.k_touch.py;
            pd->touchIsDraggingMap = true;
        } else {
            pd->touchIsDraggingMap = false;
            pd->touchIsChangingSize = false;
        }
    } else {
        // touch minimap to bring up zoomed map.
        if(pd->inputs.k_touch.py > 100 && pd->inputs.k_touch.py < 228 && pd->inputs.k_touch.px > 10 && pd->inputs.k_touch.px < 142){
            pd->mapShouldRender = true;
        }
    }
}

void tickTouchQuickSelect(PlayerData *pd) {
    if (pd->ingameMenu == MENU_NONE && !pd->mapShouldRender) {
        int i = 0;
        Inventory * inv = &(pd->inventory);
        
        for (i = 0; i < 8; ++i) {
            if((inv->lastSlot) > i) {
                int xip = i % 4;
                int yip = i / 4;
            
                if(pd->inputs.k_touch.py > 72*2+yip*21*2 && pd->inputs.k_touch.py < 72*2+yip*21*2+21*2 && pd->inputs.k_touch.px > 76*2+xip*21*2 && pd->inputs.k_touch.px < 76*2+xip*21*2+21*2) {
                    playerSetActiveItem(pd, &inv->items[i]);
                }
            }
        }
    }
}

void ingameMenuTickTouch(PlayerData *pd) {
    tickTouchMap(pd);
    tickTouchQuickSelect(pd);
}
