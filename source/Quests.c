#include "Quests.h"

#include "Globals.h"
#include "Render.h"

void initTrades() {
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

void freeTrades() {
    free(priestTrades.recipes);
    free(farmerTrades.recipes);
    free(dwarfTrades.recipes);
}

void initQuests(QuestlineManager *questManager) {
    if(questManager->questlines!=NULL) {
        freeQuests(questManager);
    }
    
    questManager->size = 2;
    questManager->questlines = (Questline*)malloc(sizeof(Questline) * (questManager->size));
}

void resetQuests(QuestlineManager *questManager) {
    int i;
    for(i=0; i<questManager->size; ++i) {
        questManager->questlines[i].currentQuest = 0;
        questManager->questlines[i].currentQuestDone = false;
    }
}

void freeQuests(QuestlineManager *questManager) {
    free(questManager->questlines);
    questManager->questlines = NULL;
}

void resetNPCMenuData(NPC_MenuData *data) {
    data->currentNPC = 0;
    data->currentNPCMenu = 0;
    data->currentNPCVal = 0;
    
    data->currentTalkSel = 0;
    data->currentTalkDone = false;
    data->currentTalkOptions = 0;
    
    data->currentTalkOption0 = "";
    data->currentTalkOption1 = "";
    data->currentTalkOption2 = "";
    data->currentTalk0 = "";
    data->currentTalk1 = "";
    data->currentTalk2 = "";
    data->currentTalk3 = "";
    data->currentTalk4 = "";
    data->currentTalk5 = "";
}

void openNPCMenu(PlayerData *pd, int npc) {
    pd->ingameMenu = MENU_NPC;
    
    NPC_MenuData *data = &(pd->npcMenuData);
    QuestlineManager *questManager = &(pd->questManager);
    
    data->currentNPC = npc;
    data->currentNPCVal = 0;
    data->currentNPCMenu = NPC_MENU_TALK;
    
    data->currentTalkSel = 0;
    data->currentTalkDone = false;
    data->currentTalkOptions = 1;
    data->currentTalkOption0 = "Bye";
    data->currentTalkOption1 = "";
    data->currentTalkOption2 = "";
    data->currentTalk0 = "";
    data->currentTalk1 = "";
    data->currentTalk2 = "";
    data->currentTalk3 = "";
    data->currentTalk4 = "";
    data->currentTalk5 = "";
    
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    switch(data->currentNPC) {
    case NPC_GIRL:
        data->currentTalkOptions = 1;
        data->currentTalkOption0 = "...";
        
        data->currentTalk0 = "Hello?";
        data->currentTalk1 = "I have a feeling of having";
        data->currentTalk2 = "forgotten something very";
        data->currentTalk3 = "important.";
        data->currentTalk4 = "Hopefully I will remember";
        data->currentTalk5 = "it soon...";
    break;
    case NPC_PRIEST:
        data->currentTalkOptions = 3;
        data->currentTalkOption1 = "Trade";
        data->currentTalkOption2 = "Why are you so few?";
        
        data->currentTalk0 = "Welcome to our small village";
        data->currentTalk1 = "I am the leader of our group.";
        data->currentTalk2 = "If you have anything usefull";
        data->currentTalk3 = "for us I might be able to";
        data->currentTalk4 = "provide something nice in";
        data->currentTalk5 = "exchange.";
    break;
    case NPC_FARMER:
        data->currentTalkOptions = 2;
        data->currentTalkOption0 = "Maybe some other time";
        data->currentTalkOption1 = "Trade";
        
        data->currentTalk0 = "Hello friend!";
        data->currentTalk1 = "Nice seeing somebody else";
        data->currentTalk2 = "visit my little farm.";
        data->currentTalk3 = "Interested in buying some";
        data->currentTalk4 = "fresh farm goods?";
        data->currentTalk5 = "";
    break;
    case NPC_LIBRARIAN:
        data->currentTalkOptions = 2;
        data->currentTalkOption0 = "Nothing";
        data->currentTalkOption1 = "What are you doing here?";
        if(questManager->questlines[1].currentQuest==1) {
            data->currentTalkOptions = 3;
            data->currentTalkOption2 = "Dwarvish language";
        }
        
        data->currentTalk0 = "Oh hello?";
        data->currentTalk1 = "You must be quite brave";
        data->currentTalk2 = "or stupid to be walking";
        data->currentTalk3 = "around in this dungeon.";
        data->currentTalk4 = "";
        data->currentTalk5 = "How can I help you?";
    break;
    case NPC_DWARF:
        if(questManager->questlines[1].currentQuest<=1) {
            questManager->questlines[1].currentQuest = 1;
            
            data->currentTalkOptions = 1;
            data->currentTalkOption0 = "?";
            
            data->currentTalk0 = "Dwo neal bet reck da lo";
            data->currentTalk1 = "dhum don lir lugn at el";
            data->currentTalk2 = "nur tor erno ur yo trad";
            data->currentTalk3 = "thra so tir kho ukk tin";
            data->currentTalk4 = "hel dro ic";
            data->currentTalk5 = "";
        //TODO: set to 2 once translation book has been bought from librarian(can only be done once it is 1, so the dwarf has been found once)
        } else if(questManager->questlines[1].currentQuest==2) {
            data->currentTalkOptions = 2;
            data->currentTalkOption0 = "Not really";
            data->currentTalkOption1 = "Trade";
            
            data->currentTalk0 = "How are ya?";
            data->currentTalk1 = "Pretty unusal meeting";
            data->currentTalk2 = "someone down here.";
            data->currentTalk3 = "";
            data->currentTalk4 = "have something valuable";
            data->currentTalk5 = "to trade?";
        }
    break;
    }
}

void tickTalkMenu(PlayerData *pd, NPC_MenuData *data) {
    if (pd->inputs.k_menu.clicked || pd->inputs.k_decline.clicked) pd->ingameMenu = MENU_NONE;
    
    if (pd->inputs.k_up.clicked){ ++data->currentTalkSel; if(data->currentTalkSel >= data->currentTalkOptions) data->currentTalkSel=0;}
    if (pd->inputs.k_down.clicked){ --data->currentTalkSel; if(data->currentTalkSel < 0) data->currentTalkSel=data->currentTalkOptions-1;}
    
    if(pd->inputs.k_accept.clicked){
        data->currentTalkDone = true;
    }
}

void tickNPCMenu(PlayerData *pd) {
    NPC_MenuData *data = &(pd->npcMenuData);
    QuestlineManager *questManager = &(pd->questManager);
    
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    if(data->currentNPCMenu==NPC_MENU_TALK) tickTalkMenu(pd, data);

    
    switch(data->currentNPC) {
    case NPC_GIRL:
        if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
            if(data->currentNPCVal==0) pd->ingameMenu = MENU_NONE;
        }
    break;
    case NPC_PRIEST:
        if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
            if(data->currentNPCVal==0) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                } else if(data->currentTalkSel==1) {
                    openCraftingMenu(pd, &priestTrades, "Trading");
                } else if(data->currentTalkSel==2) {
                    data->currentNPCVal = 1;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";
                    
                    data->currentTalk0 = "For quite some time now this";
                    data->currentTalk1 = "village has been tyrannized";
                    data->currentTalk2 = "by a powerfull Air Wizard.";
                    data->currentTalk3 = "We are the only ones who";
                    data->currentTalk4 = "still have not given up";
                    data->currentTalk5 = "our old homes.";
                }
            } else if(data->currentNPCVal==1) {
                if(data->currentTalkSel==0) {
                    data->currentNPCVal = 2;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";
                    
                    data->currentTalk0 = "Most of the time the wizard";
                    data->currentTalk1 = "hides somewhere in the";
                    data->currentTalk2 = "cloudes. They can only be";
                    data->currentTalk3 = "reached by a stairwell";
                    data->currentTalk4 = "protected by an almost";
                    data->currentTalk5 = "undestroyable stone barrier.";
                }
            } else if(data->currentNPCVal==2) {
                if(data->currentTalkSel==0) {
                    data->currentNPCVal = 3;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "...";
                    
                    data->currentTalk0 = "I am guessing you would ";
                    data->currentTalk1 = "need tools atleast as";
                    data->currentTalk2 = "strong as diamonds to be";
                    data->currentTalk3 = "able to destroy it.";
                    data->currentTalk4 = "";
                    data->currentTalk5 = "";
                }
            } else if(data->currentNPCVal==3) {
                if(data->currentTalkSel==0) {
                    data->currentNPCVal = 4;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 2;
                    data->currentTalkOption0 = "Let me do it!";
                    data->currentTalkOption1 = "I am not sure";
                    
                    data->currentTalk0 = "I am willing to give an";
                    data->currentTalk1 = "ancient artifact passed";
                    data->currentTalk2 = "down over generations to";
                    data->currentTalk3 = "anybody who manages to";
                    data->currentTalk4 = "chase the wizard away and";
                    data->currentTalk5 = "come back with proof.";
                }
            } else if(data->currentNPCVal==4) {
                pd->ingameMenu = MENU_NONE;
            }
        }
    break;
    case NPC_FARMER:
        if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
            if(data->currentNPCVal==0) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                } else if(data->currentTalkSel==1) {
                    openCraftingMenu(pd, &farmerTrades, "Trading");
                } 
            }
        }
    break;
    case NPC_LIBRARIAN:
        if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
            if(data->currentNPCVal==0) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                } else if(data->currentTalkSel==1) {
                    data->currentNPCVal = 2;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "Ok";
                    
                    data->currentTalk0 = "The books in this dungeon";
                    data->currentTalk1 = "house secrets that cannot be";
                    data->currentTalk2 = "found anywhere else in the";
                    data->currentTalk3 = "world. So I came to study";
                    data->currentTalk4 = "them. Most are written in";
                    data->currentTalk5 = "an ancient language.";
                } else if(data->currentTalkSel==2) {
                    data->currentNPCVal = 1;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 2;
                    data->currentTalkOption0 = "I need to think about it";
                    data->currentTalkOption1 = "Here they are";
                    
                    data->currentTalk0 = "So you have met a dwarf but";
                    data->currentTalk1 = "had a little communication";
                    data->currentTalk2 = "problem? I do have a dwarvish";
                    data->currentTalk3 = "translation book but I havent";
                    data->currentTalk4 = "read it yet. For 10 Gold bars";
                    data->currentTalk5 = "I will give it to you anyway.";
                }
            } else if(data->currentNPCVal==1) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                } else if(data->currentTalkSel==1) {
                    data->currentNPCVal = 2;
                    
                    data->currentTalkSel = 0;
                    data->currentTalkDone = false;
                    data->currentTalkOptions = 1;
                    data->currentTalkOption0 = "";
                    
                    if(countItemInv(ITEM_GOLDINGOT, 0, &(pd->inventory))>=10) {
                        //remove gold from player inventory
                        //TODO: Maybe I should make a generic substract items method sometime
                        Item* item = getItemFromInventory(ITEM_GOLDINGOT, &(pd->inventory));
                        item->countLevel -= 10;
                        if(item->countLevel < 1) removeItemFromInventory(item->slotNum, &(pd->inventory));
                        
                        questManager->questlines[1].currentQuest = 2;
                        
                        data->currentTalk0 = "Thank you these will be";
                        data->currentTalk1 = "really helpfull.";
                        data->currentTalk2 = "Here take this book with";
                        data->currentTalk3 = "it you should be able to";
                        data->currentTalk4 = "easily understand anything";
                        data->currentTalk5 = "a dwarf can say.";
                        
                        data->currentTalkOption0 = "Thanks";
                    } else {
                        data->currentTalk0 = "You do not seem to have";
                        data->currentTalk1 = "enough Gold Bars with you.";
                        data->currentTalk2 = "";
                        data->currentTalk3 = "Ask again when you have";
                        data->currentTalk4 = "collected the 10 Bars.";
                        data->currentTalk5 = "";
                        
                        data->currentTalkOption0 = "Ok";
                    }
                }
            } else if(data->currentNPCVal==2) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                }
            }
        }
    break;
    case NPC_DWARF:
        if(questManager->questlines[1].currentQuest<=1) {
            if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
                if(data->currentNPCVal==0) pd->ingameMenu = MENU_NONE;
            }
        } else if(questManager->questlines[1].currentQuest==2) {
            if(data->currentNPCMenu==NPC_MENU_TALK && data->currentTalkDone) {
                if(data->currentTalkSel==0) {
                    pd->ingameMenu = MENU_NONE;
                } else if(data->currentTalkSel==1) {
                    openCraftingMenu(pd, &dwarfTrades, "Trading");
                }
            }
        }
    break;
    }
}

void renderTalkMenu(NPC_MenuData *data, char * name) {
    renderFrame(1,1,24,14,0xFFFF1010);
    renderTextColor(name,24+1,14+1,0xFF000000);
    renderTextColor(name,24,14,0xFF6FE2E2);
    
    renderText(data->currentTalk0, 32, 32);
    renderText(data->currentTalk1, 32, 48);
    renderText(data->currentTalk2, 32, 64);
    renderText(data->currentTalk3, 32, 80);
    renderText(data->currentTalk4, 32, 96);
    renderText(data->currentTalk5, 32, 112);
    
    if(data->currentTalkOptions>=3) renderText(data->currentTalkOption2, 64, 147);
    if(data->currentTalkOptions>=2) renderText(data->currentTalkOption1, 64, 171);
    if(data->currentTalkOptions>=1) renderText(data->currentTalkOption0, 64, 195);
    
    if(data->currentTalkOptions>=3 && data->currentTalkSel==2) renderText(">", 48, 147);
    if(data->currentTalkOptions>=2 && data->currentTalkSel==1) renderText(">", 48, 171);
    if(data->currentTalkOptions>=1 && data->currentTalkSel==0) renderText(">", 48, 195);
}

void renderNPCMenu(NPC_MenuData *data) {
    //TODO: Handle upon currentNPC as well as the fitting quest progress
    switch(data->currentNPC) {
    case NPC_GIRL:
        if(data->currentNPCMenu==NPC_MENU_TALK) renderTalkMenu(data, "Maria");
    break;
    case NPC_PRIEST:
        if(data->currentNPCMenu==NPC_MENU_TALK) renderTalkMenu(data, "Priest Brom");
    break;
    case NPC_FARMER:
        if(data->currentNPCMenu==NPC_MENU_TALK) renderTalkMenu(data, "Farmer Garrow");
    break;
    case NPC_LIBRARIAN:
        if(data->currentNPCMenu==NPC_MENU_TALK) renderTalkMenu(data, "Librarian Ajihad");
    break;
    case NPC_DWARF:
        if(data->currentNPCMenu==NPC_MENU_TALK) renderTalkMenu(data, "Dwarf Orik");
    break;
    }
}
