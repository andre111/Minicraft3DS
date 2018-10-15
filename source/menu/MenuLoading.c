#include "MenuLoading.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

void menuLoadingTick() {
    
}

void menuLoadingRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
        
        renderText("Loading game...", (320-15*12)/2, 100);
    sf2d_end_frame();
}
