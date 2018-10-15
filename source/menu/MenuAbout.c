#include "MenuAbout.h"

#include "../Globals.h"
#include "../Render.h"
#include "../Menu.h"

void menuAboutTick() {
    if (localInputs.k_decline.clicked) {
        currentMenu = MENU_TITLE;
    }
}

void menuAboutRender() {
    /* Top Screen */
    sf2d_start_frame(GFX_TOP, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 400, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        renderText("About Minicraft",110,12);
        renderTextSized("Minicraft was made by Markus Persson for the",24,36,1.0);
        renderTextSized("22'nd ludum dare competition in december 2011.",16,48,1.0);
        renderTextSized("it is dedicated to my father. <3",72,60,1.0);
        renderTextSized("- Markus \"Notch\" Persson",104,76,1.0);
        
        renderTextColor("3DS Homebrew Edition",74,120,0xFF00FF00);
        renderTextColorSized("This port was made by David Benepe (Davideesk)",16,144,1.0,0xFF00FF00);
        renderTextColorSized("just for fun in September/October 2015.",44,156,1.0,0xFF00FF00);
        renderTextColorSized("Updated and modded by Andre Schweiger (andre111)",8,168,1.0,0xFF00FF00);
        renderTextColorSized("TY Notch for creating a fun game to remake!",28,192,1.0,0xFF00FF00);
    sf2d_end_frame();
    
    /* Bottom Screen */
    sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
        sf2d_draw_rectangle(0, 0, 320, 240, 0xFF0C0C0C); //You might think "real" black would be better, but it actually looks better that way
    
        renderTextColor("Special Thanks to:",52,12,0xFF7F7FFF);
        renderTextColor("Smea",136,50,0xFF2020FF);
        renderTextColorSized("for ctrulib",116,70,1.0,0xFF2020FF);
        renderTextColor("Xerpi",130,100,0xFFFF2020);
        renderTextColorSized("for sf2dlib",116,120,1.0,0xFFFF2020);
        renderTextColor("Music from",100,150,0xFF20FF20);
        renderTextColorSized("opengameart.org/content/",64,170,1.0,0xFF20FF20);
        renderTextColorSized("generic-8-bit-jrpg-soundtrack",48,180,1.0,0xFF20FF20);
        
        renderText("Press   to return", 58, 220);
        renderButtonIcon(localInputs.k_decline.input & -localInputs.k_decline.input, 128, 218, 1);
    sf2d_end_frame();
}
