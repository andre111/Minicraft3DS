#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "render.h"
#include "Crafting.h"

#define NPC_MENU_TALK 0

typedef struct {
    int currentQuest;
    bool currentQuestDone;
} Questline;

typedef struct {
    int size;
    Questline * questlines;
} QuestlineManager;

QuestlineManager questManager;

RecipeManager priestTrades;
RecipeManager farmerTrades;
RecipeManager dwarfTrades;

void initQuests();
void resetQuests();
void freeQuests();

void openNPCMenu(int npc);

void renderNPCMenu(int xscr, int yscr);
void tickNPCMenu();
