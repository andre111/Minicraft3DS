#pragma once

#include <3ds.h>

//only down and clicked need to be send, input is for config stuff
typedef struct {
    bool down, clicked;
    int input;
} Key;

typedef struct {
    Key k_null;
    Key k_up;
    Key k_down;
    Key k_left;
    Key k_right;
    Key k_attack;
    Key k_menu;
    Key k_pause;
    Key k_accept;
    Key k_decline;
    Key k_delete;
    Key k_menuNext;
    Key k_menuPrev;
    touchPosition k_touch;
} Inputs;

Inputs localInputs;

void tickKeys(Inputs *inputs, u32 held, u32 down);
void resetKeys(Inputs *inputs);
void resetClicked(Inputs *inputs);
bool clicked(Key key);
