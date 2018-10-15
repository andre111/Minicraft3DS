#pragma once

#define MENU_NONE 0
#define MENU_TITLE 1
#define MENU_CHOOSE_GAME 2
#define MENU_LOADGAME 3
#define MENU_MULTIPLAYER_HOST 4
#define MENU_MULTIPLAYER_JOIN 5
#define MENU_MULTIPLAYER_WAIT 6
#define MENU_LOADING 7
#define MENU_EDITOR 8
#define MENU_TUTORIAL 9
#define MENU_SETTINGS 10
#define MENU_SETTINGS_REBIND 11
#define MENU_SETTINGS_TP 12
#define MENU_ABOUT 13


#include "menu/MenuAbout.h"
#include "menu/MenuChooseGame.h"
#include "menu/MenuLoadGame.h"
#include "menu/MenuLoading.h"
#include "menu/MenuMultiplayerHost.h"
#include "menu/MenuMultiplayerJoin.h"
#include "menu/MenuMultiplayerWait.h"
#include "menu/MenuSettings.h"
#include "menu/MenuSettingsRebind.h"
#include "menu/MenuSettingsTexture.h"
#include "menu/MenuTitle.h"
#include "menu/MenuTutorial.h"

void initMenus();
void renderMenu(int menu);
void tickMenu(int menu);

void menuTickKeyboard(char * string, int maxLength);
void menuRenderKeyboard();

void menuInitMapBG();
void menuUpdateMapBG();
void menuRenderMapBGTop();
void menuRenderMapBGBottom();
