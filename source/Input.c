#include "Input.h"

void toggleKey(Key* key, bool held, bool down){
    key->down = held;
    key->clicked = down;
}
    
void tickKeys(Inputs *inputs, u32 held, u32 down){
    hidTouchRead(&(inputs->k_touch)); // Update touch position
    toggleKey(&(inputs->k_up), held & localInputs.k_up.input, down & localInputs.k_up.input);
    toggleKey(&(inputs->k_down), held & localInputs.k_down.input, down & localInputs.k_down.input);
    toggleKey(&(inputs->k_left), held & localInputs.k_left.input, down & localInputs.k_left.input);
    toggleKey(&(inputs->k_right), held & localInputs.k_right.input, down & localInputs.k_right.input);
    toggleKey(&(inputs->k_pause), held & localInputs.k_pause.input, down & localInputs.k_pause.input);
    toggleKey(&(inputs->k_attack), held & localInputs.k_attack.input, down & localInputs.k_attack.input);
    toggleKey(&(inputs->k_menu), held & localInputs.k_menu.input, down & localInputs.k_menu.input);
    toggleKey(&(inputs->k_accept), held & localInputs.k_accept.input, down & localInputs.k_accept.input);
    toggleKey(&(inputs->k_decline), held & localInputs.k_decline.input, down & localInputs.k_decline.input);
    toggleKey(&(inputs->k_delete), held & localInputs.k_delete.input, down & localInputs.k_delete.input);
    toggleKey(&(inputs->k_menuNext), held & localInputs.k_menuNext.input, down & localInputs.k_menuNext.input);
    toggleKey(&(inputs->k_menuPrev), held & localInputs.k_menuPrev.input, down & localInputs.k_menuPrev.input);
}

void resetKeys(Inputs *inputs) {
    (inputs->k_touch).px = -1;
    (inputs->k_touch).py = -1;
    
    toggleKey(&(inputs->k_up), false, false);
    toggleKey(&(inputs->k_down), false, false);
    toggleKey(&(inputs->k_left), false, false);
    toggleKey(&(inputs->k_right), false, false);
    toggleKey(&(inputs->k_pause), false, false);
    toggleKey(&(inputs->k_attack), false, false);
    toggleKey(&(inputs->k_menu), false, false);
    toggleKey(&(inputs->k_accept), false, false);
    toggleKey(&(inputs->k_decline), false, false);
    toggleKey(&(inputs->k_delete), false, false);
    toggleKey(&(inputs->k_menuNext), false, false);
    toggleKey(&(inputs->k_menuPrev), false, false);
}

void resetClicked(Inputs *inputs) {
    inputs->k_up.clicked = false;
    inputs->k_down.clicked = false;
    inputs->k_left.clicked = false;
    inputs->k_right.clicked = false;
    inputs->k_pause.clicked = false;
    inputs->k_attack.clicked = false;
    inputs->k_menu.clicked = false;
    inputs->k_accept.clicked = false;
    inputs->k_decline.clicked = false;
    inputs->k_delete.clicked = false;
    inputs->k_menuNext.clicked = false;
    inputs->k_menuPrev.clicked = false;
}
