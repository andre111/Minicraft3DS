#include "texturepack.h"

#include "ZipHelper.h"

#define MAX_FILENAME 256

bool texturepackUseDefaultIcons = true;
bool texturepackUseDefaultPlayer = true;
bool texturepackUseDefaultFont = true;
bool texturepackUseDefaultBottom = true;

void toLowerString(char * str){
    int i;
    for (i = 0; str[i] != '\0'; i++)str[i] = (char)tolower((unsigned char)str[i]);
}

int getTexturePackComment(char * filename, char * cmmtBuf) {
    // Open the zip file
    unzFile *zipfile = unzOpen(filename);
    if ( zipfile == NULL ) return 1; // Error: ZipFile could not be opened.
    
    // Get info about the zip file
    unz_global_info global_info;
    if (unzGetGlobalInfo(zipfile, &global_info ) != UNZ_OK )
    {
        unzClose( zipfile );
        return 2; // Error: Could not read global info
    }
    
    unzGetGlobalComment(zipfile, cmmtBuf, 58);

    unzClose( zipfile );

    return 0;
}

int loadTexture(char * filename) {
    char lowerFilename[MAX_FILENAME];
    strcpy(lowerFilename,filename);
    toLowerString(lowerFilename);
    
    if(strcmp(lowerFilename, "icons.png") == 0){
        if(icons!=NULL) {
            sf2d_free_texture(icons);
            icons = NULL;
        }
        
        sf2d_texture* tex = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);
        if(tex == NULL){
            return 0;
        }
        
        icons = tex;
        reloadColors();
       
        texturepackUseDefaultIcons = false;
    } else if(strcmp(lowerFilename, "player.png") == 0){
        if(playerSprites!=NULL) {
            sf2d_free_texture(playerSprites);
            playerSprites = NULL;
        }
        
        sf2d_texture* tex = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);
        if(tex == NULL){
            return 0;
        }
        
        playerSprites = tex;
        
        texturepackUseDefaultPlayer = false;
    } else if(strcmp(lowerFilename, "font.png") == 0){
        if(font!=NULL) {
            sf2d_free_texture(font);
            font = NULL;
        }
        
        sf2d_texture* tex = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);
        if(tex == NULL){
            return 0;
        }
        
        font = tex;
        
        texturepackUseDefaultFont = false;
    } else if(strcmp(lowerFilename, "bottombg.png") == 0){
        if(bottombg!=NULL) {
            sf2d_free_texture(bottombg);
            bottombg = NULL;
        }
        
        sf2d_texture* tex = sfil_load_PNG_file(filename, SF2D_PLACE_RAM);
        if(tex == NULL){
            return 0;
        }
        
        bottombg = tex;
        
        texturepackUseDefaultBottom = false;
    }
    
    return 0;
}

int loadTexturePack(char * filename) {
    texturepackUseDefaultIcons = true;
    texturepackUseDefaultPlayer = true; 
    texturepackUseDefaultFont = true;
    texturepackUseDefaultBottom = true;

    if(unzipAndLoad(filename, &loadTexture, NULL, ZIPHELPER_CLEANUP_FILES)!=0) {
        return 1;
    }
    
    if(texturepackUseDefaultIcons){
        if(icons!=NULL) {
            sf2d_free_texture(icons);
            icons = NULL;
        }
        icons = sfil_load_PNG_buffer(icons_png, SF2D_PLACE_RAM);
        reloadColors();
    }
    if(texturepackUseDefaultPlayer) {
        if(playerSprites!=NULL) {
            sf2d_free_texture(playerSprites);
            playerSprites = NULL;
        }
        playerSprites = sfil_load_PNG_buffer(player_png, SF2D_PLACE_RAM);
    }
    if(texturepackUseDefaultFont) {
        if(font!=NULL) {
            sf2d_free_texture(font);
            font = NULL;
        }
        font = sfil_load_PNG_buffer(Font_png, SF2D_PLACE_RAM);
    }
    if(texturepackUseDefaultBottom) {
        if(bottombg!=NULL) {
            sf2d_free_texture(bottombg);
            bottombg = NULL;
        }
        bottombg = sfil_load_PNG_buffer(bottombg_png, SF2D_PLACE_RAM);
    }

    return 0;
}
