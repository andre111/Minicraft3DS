#include "Quests.h"

u8 currentNPC;

int currentNPCMenu;
int currentNPCVal;

int currentTalkSel;
bool currentTalkDone;
int currentTalkOptions;
char * currentTalkOption0;
char * currentTalkOption1;
char * currentTalkOption2;
char * currentTalk0;
char * currentTalk1;
char * currentTalk2;
char * currentTalk3;
char * currentTalk4;
char * currentTalk5;

void initQuests() {
    questManager.size = 2;
    questManager.questlines = (Questline*)malloc(sizeof(Questline) * (questManager.size));
    
    priestTrades.size = 5;
    priestTrades.recipes = (Recipe*)malloc(sizeof(Recipe) * (priestTrades.size));
    priestTrades.recipes[0] = defineRecipe(ITEM_DUNGEON_KEY,1,1,ITEM_MAGIC_DUST,2);
	priestTrades.recipes[1] = defineRecipe(ITEM_WIZARD_SUMMON,1,4,ITEM_CLOUD,100,ITEM_IRONINGOT,10,ITEM_BONE,10,ITEM_LEATHER,10);
    priestTrades.recipes[2] = defineRecipe(TOOL_MAGIC_COMPASS,1,2,ITEM_IRONINGOT,10,ITEM_GLASS,5);
    priestTrades.recipes[3] = defineRecipe(ITEM_COIN,1,1,ITEM_SLIME,5);
    priestTrades.recipes[4] = defineRecipe(ITEM_COIN,1,1,ITEM_FLESH,5);
    
    farmerTrades.size = 7;
    farmerTrades.recipes = (Recipe*)malloc(sizeof(Recipe) * (farmerTrades.size));
    farmerTrades.recipes[0] = defineRecipe(ITEM_WHEAT,5,1,ITEM_COIN,3);
    farmerTrades.recipes[1] = defineRecipe(ITEM_BREAD,1,1,ITEM_COIN,3);
    farmerTrades.recipes[2] = defineRecipe(ITEM_APPLE,2,1,ITEM_COIN,4);
    farmerTrades.recipes[3] = defineRecipe(ITEM_ACORN,3,1,ITEM_COIN,1);
    farmerTrades.recipes[4] = defineRecipe(ITEM_SEEDS,4,1,ITEM_COIN,2);
    farmerTrades.recipes[5] = defineRecipe(ITEM_COIN,2,1,ITEM_SEEDS,5);
    farmerTrades.recipes[6] = defineRecipe(ITEM_COIN,1,1,ITEM_ACORN,5);
    
    dwarfTrades.size = 2;
    dwarfTrades.recipes = (Recipe*)malloc(sizeof(Recipe) * (dwarfTrades.size));
    dwarfTrades.recipes[0] = defineRecipe(ITEM_IRONINGOT,4,1,ITEM_GOLDINGOT,1);
    dwarfTrades.recipes[1] = defineRecipe(ITEM_GOLDINGOT,2,1,ITEM_GEM,1);
    //TODO: Trade Dragon Scales for something really nice
}

void resetQuests() {
    int i;
    for(i=0; i<questManager.size; ++i) {
        questManager.questlines[i].currentQuest = 0;
        questManager.questlines[i].currentQuestDone = false;
    }
}

void freeQuests() {
    free(questManager.questlines);
    free(priestTrades.recipes);
    free(farmerTrades.recipes);
    free(dwarfTrades.recipes);
}

void openNPCMenu(int npc) {
    currentNPC = npc;
    currentNPCVal = 0;
    currentMenu = MENU_NPC;
    currentNPCMenu = NPC_MENU_TALK;
    
    currentTalkSel = 0;
    currentTalkDone = false;
    currentTalkOptions = 1;
    currentTalkOption0 = "Bye";
    currentTalkOption1 = "";
    currentTalkOption2 = "";
    currentTalk0 = "";
    currentTalk1 = "";
    currentTalk2 = "";
    currentTalk3 = "";
    currentTalk4 = "";
    currentTalk5 = "";
    
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    switch(currentNPC) {
    case NPC_GIRL:
        
    break;
    case NPC_PRIEST:
        currentTalkOptions = 3;
        currentTalkOption1 = "Trade";
        currentTalkOption2 = "Why are you so few?";
        
        currentTalk0 = "Welcome to our small village";
        currentTalk1 = "I am the leader of our group.";
        currentTalk2 = "If you have anything usefull";
        currentTalk3 = "for us I might be able to";
        currentTalk4 = "provide something nice in";
        currentTalk5 = "exchange.";
    break;
    case NPC_FARMER:
        currentTalkOptions = 2;
        currentTalkOption0 = "Maybe some other time";
        currentTalkOption1 = "Trade";
        
        currentTalk0 = "Hello friend!";
        currentTalk1 = "Nice seeing somebody else";
        currentTalk2 = "visit my little farm.";
        currentTalk3 = "Interested in buying some";
        currentTalk4 = "fresh farm goods?";
        currentTalk5 = "";
    break;
    case NPC_LIBRARIAN:
        currentTalkOptions = 2;
        currentTalkOption0 = "Nothing";
        currentTalkOption1 = "What are you doing here?";
        if(questManager.questlines[1].currentQuest==1) {
            currentTalkOptions = 3;
            currentTalkOption2 = "Dwarvish language";
        }
        
        currentTalk0 = "Oh hello?";
        currentTalk1 = "You must be quite brave";
        currentTalk2 = "or stupid to be walking";
        currentTalk3 = "around in this dungeon.";
        currentTalk4 = "";
        currentTalk5 = "How can I help you?";
    break;
    case NPC_DWARF:
        if(questManager.questlines[1].currentQuest<=1) {
            questManager.questlines[1].currentQuest = 1;
            
            currentTalkOptions = 1;
            currentTalkOption0 = "?";
            
            currentTalk0 = "Dwo neal bet reck da lo";
            currentTalk1 = "dhum don lir lugn at el";
            currentTalk2 = "nur tor erno ur yo trad";
            currentTalk3 = "thra so tir kho ukk tin";
            currentTalk4 = "hel dro ic";
            currentTalk5 = "";
        //TODO: set to 2 once translation book has been bought from librarian(can only be done once it is 1, so the dwarf has been found once)
        } else if(questManager.questlines[1].currentQuest==2) {
            currentTalkOptions = 2;
            currentTalkOption0 = "Not really";
            currentTalkOption1 = "Trade";
            
            currentTalk0 = "How are ya?";
            currentTalk1 = "Pretty unusal meeting a";
            currentTalk2 = "human down here.";
            currentTalk3 = "";
            currentTalk4 = "have something valuable";
            currentTalk5 = "to trade?";
        }
    break;
    }
}

void tickTalkMenu() {
    if (k_menu.clicked || k_decline.clicked) currentMenu = MENU_NONE;
	
    if (k_up.clicked){ ++currentTalkSel; if(currentTalkSel >= currentTalkOptions) currentTalkSel=0;}
    if (k_down.clicked){ --currentTalkSel; if(currentTalkSel < 0) currentTalkSel=currentTalkOptions-1;}
    
    if(k_accept.clicked){
        currentTalkDone = true;
    }
}

void tickNPCMenu() {
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    if(currentNPCMenu==NPC_MENU_TALK) tickTalkMenu();

    
    switch(currentNPC) {
    case NPC_GIRL:
        
    break;
    case NPC_PRIEST:
        if(currentNPCMenu==NPC_MENU_TALK && currentTalkDone) {
            if(currentNPCVal==0) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                } else if(currentTalkSel==1) {
                    currentRecipes = &priestTrades;
                    currentCraftTitle = "Trading";
                    currentMenu = MENU_CRAFTING; 
                    checkCanCraftRecipes(currentRecipes, player.p.inv);
                    sortRecipes(currentRecipes);
                } else if(currentTalkSel==2) {
                    currentNPCVal = 1;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 1;
                    currentTalkOption0 = "...";
                    
                    currentTalk0 = "For quite some time now this";
                    currentTalk1 = "village has been tyrannized";
                    currentTalk2 = "by a powerfull Air Wizard.";
                    currentTalk3 = "We are the only ones who";
                    currentTalk4 = "still have not given up";
                    currentTalk5 = "our old homes.";
                }
            } else if(currentNPCVal==1) {
                if(currentTalkSel==0) {
                    currentNPCVal = 2;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 1;
                    currentTalkOption0 = "...";
                    
                    currentTalk0 = "Most of the time the wizard";
                    currentTalk1 = "hides somewhere in the";
                    currentTalk2 = "cloudes. They can only be";
                    currentTalk3 = "reached by a stairwell";
                    currentTalk4 = "protected by an almost";
                    currentTalk5 = "undestroyable stone barrier.";
                }
            } else if(currentNPCVal==2) {
                if(currentTalkSel==0) {
                    currentNPCVal = 3;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 1;
                    currentTalkOption0 = "...";
                    
                    currentTalk0 = "I am guessing you would ";
                    currentTalk1 = "need tools atleast as";
                    currentTalk2 = "strong as diamonds to be";
                    currentTalk3 = "able to destroy it.";
                    currentTalk4 = "";
                    currentTalk5 = "";
                }
            } else if(currentNPCVal==3) {
                if(currentTalkSel==0) {
                    currentNPCVal = 4;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 2;
                    currentTalkOption0 = "Let me do it!";
                    currentTalkOption1 = "I am not sure";
                    
                    currentTalk0 = "I am willing to give an";
                    currentTalk1 = "ancient artifact passed";
                    currentTalk2 = "down over generations to";
                    currentTalk3 = "anybody who manages to";
                    currentTalk4 = "chase the wizard away and";
                    currentTalk5 = "come back with proof.";
                }
            } else if(currentNPCVal==4) {
                currentMenu = MENU_NONE;
            }
        }
    break;
    case NPC_FARMER:
        if(currentNPCMenu==NPC_MENU_TALK && currentTalkDone) {
            if(currentNPCVal==0) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                } else if(currentTalkSel==1) {
                    currentRecipes = &farmerTrades;
                    currentCraftTitle = "Trading";
                    currentMenu = MENU_CRAFTING; 
                    checkCanCraftRecipes(currentRecipes, player.p.inv);
                    sortRecipes(currentRecipes);
                } 
            }
        }
    break;
    case NPC_LIBRARIAN:
        if(currentNPCMenu==NPC_MENU_TALK && currentTalkDone) {
            if(currentNPCVal==0) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                } else if(currentTalkSel==1) {
                    currentNPCVal = 2;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 1;
                    currentTalkOption0 = "Ok";
                    
                    currentTalk0 = "The books in this dungeon";
                    currentTalk1 = "house secrets that cannot be";
                    currentTalk2 = "found anywhere else in the";
                    currentTalk3 = "world. So I came to study";
                    currentTalk4 = "them. Most are written in";
                    currentTalk5 = "an ancient language.";
                } else if(currentTalkSel==2) {
                    currentNPCVal = 1;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 2;
                    currentTalkOption0 = "I need to think about it";
                    currentTalkOption1 = "Here they are";
                    
                    currentTalk0 = "So you have met a dwarf but";
                    currentTalk1 = "had a little communication";
                    currentTalk2 = "problem? I do have a dwarvish";
                    currentTalk3 = "translation book but I havent";
                    currentTalk4 = "read it yet. For 10 Gold bars";
                    currentTalk5 = "I will give it to you anyway.";
                }
            } else if(currentNPCVal==1) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                } else if(currentTalkSel==1) {
                    currentNPCVal = 2;
                    
                    currentTalkSel = 0;
                    currentTalkDone = false;
                    currentTalkOptions = 1;
                    currentTalkOption0 = "";
                    
                    if(countItemInv(ITEM_GOLDINGOT,0,player.p.inv)>=10) {
                        //remove gold from player inventory
                        //TODO: Maybe I should make a generic substract items method sometime
                        Item* item = getItemFromInventory(ITEM_GOLDINGOT, player.p.inv);
                        item->countLevel -= 10;
                        if(item->countLevel < 1) removeItemFromInventory(item->slotNum, player.p.inv);
                        
                        questManager.questlines[1].currentQuest = 2;
                        
                        currentTalk0 = "Thank you these will be";
                        currentTalk1 = "really helpfull.";
                        currentTalk2 = "Here take this book with";
                        currentTalk3 = "it you should be able to";
                        currentTalk4 = "easily understand anything";
                        currentTalk5 = "a dwarf can say.";
                        
                        currentTalkOption0 = "Thanks";
                    } else {
                        currentTalk0 = "You do not seem to have";
                        currentTalk1 = "enough Gold Bars with you.";
                        currentTalk2 = "";
                        currentTalk3 = "Ask again when you have";
                        currentTalk4 = "collected the 10 Bars.";
                        currentTalk5 = "";
                        
                        currentTalkOption0 = "Ok";
                    }
                }
            } else if(currentNPCVal==2) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                }
            }
        }
    break;
    case NPC_DWARF:
        if(questManager.questlines[1].currentQuest<=1) {
            if(currentNPCMenu==NPC_MENU_TALK && currentTalkDone) {
                if(currentNPCVal==0) currentMenu = MENU_NONE;
            }
        } else if(questManager.questlines[1].currentQuest==2) {
            if(currentNPCMenu==NPC_MENU_TALK && currentTalkDone) {
                if(currentTalkSel==0) {
                    currentMenu = MENU_NONE;
                } else if(currentTalkSel==1) {
                    currentRecipes = &dwarfTrades;
                    currentCraftTitle = "Trading";
                    currentMenu = MENU_CRAFTING; 
                    checkCanCraftRecipes(currentRecipes, player.p.inv);
                    sortRecipes(currentRecipes);
                }
            }
        }
    break;
    }
}

void renderTalkMenu(char * name) {
    renderFrame(1,1,24,14,0xFFFF1010);
    drawTextColor(name,24+1,14+1,0xFF000000);
	drawTextColor(name,24,14,0xFF6FE2E2);
    
    drawText(currentTalk0, 32, 32);
    drawText(currentTalk1, 32, 48);
    drawText(currentTalk2, 32, 64);
    drawText(currentTalk3, 32, 80);
    drawText(currentTalk4, 32, 96);
    drawText(currentTalk5, 32, 112);
    
    if(currentTalkOptions>=3) drawText(currentTalkOption2, 64, 147);
    if(currentTalkOptions>=2) drawText(currentTalkOption1, 64, 171);
    if(currentTalkOptions>=1) drawText(currentTalkOption0, 64, 195);
    
    if(currentTalkOptions>=3 && currentTalkSel==2) drawText(">", 48, 147);
    if(currentTalkOptions>=2 && currentTalkSel==1) drawText(">", 48, 171);
    if(currentTalkOptions>=1 && currentTalkSel==0) drawText(">", 48, 195);
}

void renderNPCMenu(int xscr, int yscr) {
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    switch(currentNPC) {
    case NPC_GIRL:
        if(currentNPCMenu==NPC_MENU_TALK) renderTalkMenu("TODO");
    break;
    case NPC_PRIEST:
        if(currentNPCMenu==NPC_MENU_TALK) renderTalkMenu("Priest Brom");
    break;
    case NPC_FARMER:
        if(currentNPCMenu==NPC_MENU_TALK) renderTalkMenu("Farmer Garrow");
    break;
    case NPC_LIBRARIAN:
        if(currentNPCMenu==NPC_MENU_TALK) renderTalkMenu("Librarian Ajihad");
    break;
    case NPC_DWARF:
        if(currentNPCMenu==NPC_MENU_TALK) renderTalkMenu("Dwarf Orik");
    break;
    }
}
