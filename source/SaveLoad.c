#include "SaveLoad.h"

#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include "ZipHelper.h"

//TODO: Loading of most older saves is broken, and even copying tje loading code from a working release (1.5.2) does not fix this problem, but the release itself works

bool entityIsImportant(Entity * e){
    switch(e->type){
        case ENTITY_AIRWIZARD:
        case ENTITY_SLIME:
        case ENTITY_ZOMBIE:
        case ENTITY_SKELETON:
        case ENTITY_KNIGHT:
        case ENTITY_ITEM:
        case ENTITY_FURNITURE:
        case ENTITY_PASSIVE:
        case ENTITY_GLOWWORM:
        case ENTITY_DRAGON:
        case ENTITY_NPC:
            return true;
        default:
            return false;
    }
}

s16 calculateImportantEntites(EntityManager * eManager, int level){
    int i;
    s16 count = 0;
    for(i = 0; i < eManager->lastSlot[level]; ++i){
        if(entityIsImportant(&eManager->entities[level][i])){
            count++;
        }
    }
    return count;
}

//helper methods
char **files;
int fileCount;

void saveTrackFileReset() {
    if(files!=NULL) {
        for(int i=0; i<fileCount; i++) {
            free(files[i]);
        }
        free(files);
    }
    
    files = NULL;
    fileCount = 0;
}

int saveTrackFile(char *filename) {
    //test if entry allready present
    for(int i=0; i<fileCount; i++) {
        if(strcmp(filename, files[i])==0) {
            return 0;
        }
    }
    
    //add new entry
    fileCount++;
    char **newFiles = realloc(files, fileCount*sizeof(char*));
    if(!newFiles) {
        for(int i=0; i<fileCount-1; i++) {
            free(files[i]);
        }
        free(files);
        files = NULL;
        return 1;
    }
    
    files = newFiles;
    files[fileCount-1] = malloc(strlen(filename)+1);
    strcpy(files[fileCount-1], filename);
    return 0;
}

void saveDeleteTrackedFiles() {
    for(int i=0; i<fileCount; i++) {
        remove(files[i]);
    }
}

bool saveFileCopy(char *target, char *source) {
    char *buffer = malloc(SAVE_COPYBUFFER_SIZE);
    if(buffer==NULL) {
        return false;
    }
    
    FILE *in = fopen(source, "rb");
    if(in==NULL) {
        free(buffer);
        return false;
    }
    FILE *out = fopen(target, "wb");
    if(out==NULL) {
        fclose(out);
        free(buffer);
        return false;
    }
    
    size_t size;
    do {
        size = fread(buffer, 1, SAVE_COPYBUFFER_SIZE, in);
        
        if(size>0) {
            fwrite(buffer, 1, size, out);
        }
    } while(size>0);
    
    fclose(in);
    fclose(out);
    free(buffer);
    
    return true;
}

//internal save methods
void saveInventory(Inventory *inv, EntityManager *eManager, FILE *file) {
    fwrite(&inv->lastSlot, sizeof(s16), 1, file); // write amount of items in inventory;
    for(int j = 0; j < inv->lastSlot; ++j) {
        fwrite(&(inv->items[j].id), sizeof(s16), 1, file); // write ID of item
        fwrite(&(inv->items[j].countLevel), sizeof(s16), 1, file); // write count/level of item
        if(inv->items[j].id == ITEM_CHEST){
            int invIndex = inv->items[j].chestPtr - eManager->invs;
            fwrite(&invIndex, sizeof(int), 1, file);
        }
    }
}

void saveEntity(Entity *e, EntityManager *eManager, FILE *file) {
    fwrite(&e->type, sizeof(s16), 1, file); // write entity's type ID
    fwrite(&e->x, sizeof(s16), 1, file); // write entity's x coordinate
    fwrite(&e->y, sizeof(s16), 1, file); // write entity's y coordinate
    switch(e->type){
        case ENTITY_AIRWIZARD:
            fwrite(&e->wizard.health, sizeof(s16), 1, file);
            break;
        case ENTITY_ZOMBIE:
        case ENTITY_SKELETON:
        case ENTITY_KNIGHT:
        case ENTITY_SLIME:
            fwrite(&e->hostile.health, sizeof(s16), 1, file);
            fwrite(&e->hostile.lvl, sizeof(s8), 1, file);
            break;
        case ENTITY_ITEM:
            fwrite(&e->entityItem.item.id, sizeof(s16), 1, file);
            fwrite(&e->entityItem.item.countLevel, sizeof(s16), 1, file);
            fwrite(&e->entityItem.age, sizeof(s16), 1, file);
            break;
        case ENTITY_FURNITURE:
            fwrite(&e->entityFurniture.itemID, sizeof(s16), 1, file);
            int invIndex = e->entityFurniture.inv - eManager->invs;
            fwrite(&invIndex, sizeof(int), 1, file);
            break;
        case ENTITY_PASSIVE:
            fwrite(&e->passive.health, sizeof(s16), 1, file);
            fwrite(&e->passive.mtype, sizeof(u8), 1, file);
            break;
        case ENTITY_DRAGON:
            fwrite(&e->dragon.health, sizeof(s16), 1, file);
            break;
        case ENTITY_NPC:
            fwrite(&e->npc.type, sizeof(u8), 1, file);
            break;
    }
}

void saveWorldInternal(char *filename, EntityManager *eManager, WorldData *worldData) {
    FILE * file = fopen(filename, "wb"); //TODO: should be checked
    
    int i, j;
    
    //write savefile version
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, file);
    
    // Inventory Data
    fwrite(&eManager->nextInv, sizeof(s16), 1, file); // write amount of inventories.
    for(i = 0; i < eManager->nextInv; ++i) {
        saveInventory(&(eManager->invs[i]), eManager, file);
    }
    
    // Entity Data
    for(i = 0; i < 5; ++i) { //for every level (except dungeon of course)
        int amount = calculateImportantEntites(eManager,i);
        fwrite(&amount, sizeof(s16), 1, file); // read amount of entities in level.
        for(j = 0; j < eManager->lastSlot[i]; ++j){
            if(!entityIsImportant(&eManager->entities[i][j])) continue;
            
            saveEntity(&eManager->entities[i][j], eManager, file);
        }
    }
    
    // Day/season Data
    fwrite(&worldData->daytime, sizeof(u16), 1, file);
    fwrite(&worldData->day, sizeof(int), 1, file);
    fwrite(&worldData->season, sizeof(u8), 1, file);
    fwrite(&worldData->rain, sizeof(bool), 1, file);
    
    // Compass Data
    fwrite(worldData->compassData, sizeof(u8), 6*3, file); //x,y of choosen stair and count per level
    
    // Map Data
    //Don't write or load dungeon, so only first 5 levels not 6
    fwrite(worldData->map, sizeof(u8), 128*128*5, file); // Map Tile IDs, 128*128*5 bytes = 80KB
    fwrite(worldData->data, sizeof(u8), 128*128*5, file); // Map Tile Data (Damage done to trees/rocks, age of wheat & saplings, etc). 80KB
    
    fclose(file);
}

void savePlayerInternal(char *filename, PlayerData *player, EntityManager *eManager) {
    FILE * file = fopen(filename, "wb"); //TODO: should be checked
    
    int i;
    
    //write savefile version
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, file);
    
    // basic player info
    fwrite(&player->score, sizeof(int), 1, file);
    fwrite(&player->isSpawned, sizeof(bool), 1, file);
    fwrite(&player->entity.p.hasWonSaved, sizeof(bool), 1, file);
    fwrite(&player->entity.p.health, sizeof(s16), 1, file);
    fwrite(&player->entity.x, sizeof(s16), 1, file);
    fwrite(&player->entity.y, sizeof(s16), 1, file);
    fwrite(&player->entity.level, sizeof(s8), 1, file);
    
    saveInventory(&(player->inventory), eManager, file);
    
    // Sprite info
    fwrite(&(player->sprite.choosen), sizeof(bool), 1, file);
    fwrite(&(player->sprite.legs), sizeof(u8), 1, file);
    fwrite(&(player->sprite.body), sizeof(u8), 1, file);
    fwrite(&(player->sprite.arms), sizeof(u8), 1, file);
    fwrite(&(player->sprite.head), sizeof(u8), 1, file);
    fwrite(&(player->sprite.eyes), sizeof(u8), 1, file);
    fwrite(&(player->sprite.accs), sizeof(u8), 1, file);
    
    // Effect Data
    int esize = EFFECTS_MAX;
    fwrite(&esize, sizeof(int), 1, file);
    for(i = 0; i < EFFECTS_MAX; i++) {
        fwrite(&(player->effects[i].level), sizeof(u8), 1, file);
        fwrite(&(player->effects[i].time), sizeof(u32), 1, file);
    }
    
    // Minimap Data
    fwrite(player->minimapData, sizeof(u8), 128*128, file); // Minimap, visibility data 16KB
    
    // Quest Data
    fwrite(&(player->questManager.size), sizeof(int), 1, file);
    for(i = 0; i < player->questManager.size; ++i) {
        fwrite(&(player->questManager.questlines[i].currentQuest), sizeof(int), 1, file);
        fwrite(&(player->questManager.questlines[i].currentQuestDone), sizeof(bool), 1, file);
    }
    
    fclose(file);
}

//internal load methods
void loadInventory(Inventory *inv, EntityManager *eManager, FILE *file, int version) {
    fread(&(inv->lastSlot), sizeof(s16), 1, file); // read amount of items in inventory;
    for(int j = 0; j < inv->lastSlot; ++j) {
        fread(&(inv->items[j].id), sizeof(s16), 1, file); // write ID of item
        fread(&(inv->items[j].countLevel), sizeof(s16), 1, file); // write count/level of item
        if(version<=1) { //read legacy value
            bool onlyOne;
            fread(&onlyOne, sizeof(bool), 1, file);
        }
        
        inv->items[j].invPtr = (int*)inv; // setup Inventory pointer
        inv->items[j].slotNum = j; // setup slot number
        if(inv->items[j].id == ITEM_CHEST){ // for chest item specifically.
            int invIndex; 
            fread(&invIndex, sizeof(int), 1, file);
            inv->items[j].chestPtr = (Inventory*)&eManager->invs[invIndex]; // setup chest inventory pointer.
        }
    }
}

void loadEntity(Entity *e, int level, int j, EntityManager *eManager, FILE *file, int version) {
    s16 type;
    s16 x;
    s16 y;
    
    s16 health;
    int lvl;
    
    s16 itemID;
    int invIndex;
    
    fread(&type, sizeof(s16), 1, file); // read entity's type ID
    fread(&x, sizeof(s16), 1, file); // read entity's x coordinate
    fread(&y, sizeof(s16), 1, file); // read entity's y coordinate
    switch(type){
        case ENTITY_AIRWIZARD:
            *e = newEntityAirWizard(x, y, level);
            fread(&e->wizard.health, sizeof(s16), 1, file);
            break;
        case ENTITY_SLIME:
            fread(&health, sizeof(s16), 1, file);
            fread(&lvl, sizeof(s8), 1, file);
            *e = newEntitySlime(lvl, x, y, level);
            e->hostile.health = health;
            break;
        case ENTITY_ZOMBIE:
            fread(&health, sizeof(s16), 1, file);
            fread(&lvl, sizeof(s8), 1, file);
            *e = newEntityZombie(lvl, x, y, level);
            e->hostile.health = health;
            break;
        case ENTITY_SKELETON:
            fread(&health, sizeof(s16), 1, file);
            fread(&lvl, sizeof(s8), 1, file);
            *e = newEntitySkeleton(lvl, x, y, level);
            e->hostile.health = health;
            break;
        case ENTITY_KNIGHT:
            fread(&health, sizeof(s16), 1, file);
            fread(&lvl, sizeof(s8), 1, file);
            *e = newEntityKnight(lvl, x, y, level);
            e->hostile.health = health;
            break;
        case ENTITY_ITEM:
            *e = newEntityItem(newItem(0, 0), x, y, level);
            fread(&e->entityItem.item.id, sizeof(s16), 1, file);
            fread(&e->entityItem.item.countLevel, sizeof(s16), 1, file);
            fread(&e->entityItem.age, sizeof(s16), 1, file);
            break;
        case ENTITY_FURNITURE:
            fread(&itemID, sizeof(s16), 1, file);
            fread(&invIndex, sizeof(int), 1, file);
            
            *e = newEntityFurniture(itemID, &eManager->invs[invIndex], x, y, level);
            break;
        case ENTITY_PASSIVE:
            *e = newEntityPassive(0, x, y, level);
            fread(&e->passive.health, sizeof(s16), 1, file);
            fread(&e->passive.mtype, sizeof(u8), 1, file);
            break;
        case ENTITY_GLOWWORM:
            *e = newEntityGlowworm(x, y, level);
            break;
        case ENTITY_DRAGON:
            *e = newEntityDragon(x, y, level);
            fread(&e->dragon.health, sizeof(s16), 1, file);
            break;
        case ENTITY_NPC:
            *e = newEntityNPC(0, x, y, level);
            fread(&e->npc.type, sizeof(u8), 1, file);
            break;
    }
    e->type = type;
    e->x = x;
    e->y = y;
    e->slotNum = j;
}

void loadWorldInternal(char *filename, EntityManager *eManager, WorldData *worldData) {
    FILE * file = fopen(filename, "rb"); //TODO: should be checked
    
    int i, j;
    
    //read savefile version
    int version;
    fread(&version, sizeof(int), 1, file);
    
    // Inventory Data
    fread(&eManager->nextInv, sizeof(s16), 1, file);
    for(i = 0; i < eManager->nextInv; ++i) {
        loadInventory(&(eManager->invs[i]), eManager, file, version);
    }
    
    // Entity Data
    for(i = 0; i < 5; ++i) {
        fread(&eManager->lastSlot[i], sizeof(s16), 1, file); // read amount of entities in level.
        for(j = 0; j < eManager->lastSlot[i]; ++j) {
            loadEntity(&eManager->entities[i][j], i, j, eManager, file, version);
        }
    }
    
    // Day/season Data
    fread(&worldData->daytime, sizeof(u16), 1, file);
    fread(&worldData->day, sizeof(int), 1, file);
    fread(&worldData->season, sizeof(u8), 1, file);
    fread(&worldData->rain, sizeof(bool), 1, file);
    
    // Compass Data
    fread(worldData->compassData, sizeof(u8), 6*3, file); //x,y of choosen stair and count per level
    
    // Map Data
    //Don't write or load dungeon, so only first 5 levels not 6
    fread(worldData->map, sizeof(u8), 128*128*5, file); // Map Tile IDs, 128*128*5 bytes = 80KB
    fread(worldData->data, sizeof(u8), 128*128*5, file); // Map Tile Data (Damage done to trees/rocks, age of wheat & saplings, etc). 80KB
    
    fclose(file);
}

void loadPlayerInternal(char *filename, PlayerData *player, EntityManager *eManager) {
    FILE * file = fopen(filename, "rb"); //TODO: should be checked
    
    int i;
    
    //read savefile version
    int version;
    fread(&version, sizeof(int), 1, file);
    
    // basic player info
    fread(&player->score, sizeof(int), 1, file);
    fread(&player->isSpawned, sizeof(bool), 1, file);
    fread(&player->entity.p.hasWonSaved, sizeof(bool), 1, file);
    fread(&player->entity.p.health, sizeof(s16), 1, file);
    fread(&player->entity.x, sizeof(s16), 1, file);
    fread(&player->entity.y, sizeof(s16), 1, file);
    fread(&player->entity.level, sizeof(s8), 1, file);
    
    loadInventory(&(player->inventory), eManager, file, version);
    
    // Sprite info
    fread(&(player->sprite.choosen), sizeof(bool), 1, file);
    fread(&(player->sprite.legs), sizeof(u8), 1, file);
    fread(&(player->sprite.body), sizeof(u8), 1, file);
    fread(&(player->sprite.arms), sizeof(u8), 1, file);
    fread(&(player->sprite.head), sizeof(u8), 1, file);
    fread(&(player->sprite.eyes), sizeof(u8), 1, file);
    if(version>=2) fread(&(player->sprite.accs), sizeof(s8), 1, file);
    
    // Effect Data
    if(version>=2) {
        int esize;
        fread(&esize, sizeof(int), 1, file);
        for(i = 0; i < esize; i++) {
            fread(&(player->effects[i].level), sizeof(u8), 1, file);
            fread(&(player->effects[i].time), sizeof(u32), 1, file);
        }
    }
    
    // Minimap Data
    fread(player->minimapData, sizeof(u8), 128*128, file); // Minimap, visibility data 16KB
    
    // Quest Data
    fread(&(player->questManager.size), sizeof(int), 1, file);
    for(i = 0; i < player->questManager.size; ++i) {
        fread(&(player->questManager.questlines[i].currentQuest), sizeof(int), 1, file);
        fread(&(player->questManager.questlines[i].currentQuestDone), sizeof(bool), 1, file);
    }
    
    fclose(file);
}


bool saveWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount) {
    //check if old save file exists
    bool exists = false;
    FILE *testFile = fopen(filename, "rb");
    if(testFile) {
        fclose(testFile);
        exists = true;
    }
    
    saveTrackFileReset();
    
    if(exists) {
        //create backup copy
        char *filenameBackup = malloc(sizeof(filename)+4+1);
        if(filenameBackup==NULL) {
            return false;
        }
        strcpy(filenameBackup, filename);
        strcat(filenameBackup, ".bak");
        if(!saveFileCopy(filenameBackup, filename)) {
            return false;
        }
    
        //extract files and keep track of references
        if(unzipAndLoad(filename, &saveTrackFile, SAVE_COMMENT, ZIPHELPER_KEEP_FILES)!=0) {
            saveDeleteTrackedFiles();
            return false;
        }
        
        remove(filename);
    }
    
    //save world data
    saveWorldInternal("main.wld", eManager, worldData);
    saveTrackFile("main.wld");
    
    //save player data of active players
    for(int i=0; i<playerCount; i++) {
        char playerFilename[50];
        playerFilename[0] = '\0';
        sprintf(playerFilename, "%lu.plr", players[i].id);
        
        savePlayerInternal(playerFilename, players+i, eManager);
        saveTrackFile(playerFilename);
    }
    
    //zip all tracked files
    if(zipFiles(filename, files, fileCount, ZIPHELPER_REPLACE, SAVE_COMMENT)!=0) {
        remove(filename);
    }
    
    saveDeleteTrackedFiles();
    
    return true;
}

bool loadHadWorld;
EntityManager *loadEManager;
WorldData *loadWorldData;
PlayerData *loadPlayers;
int loadPlayerCount;

int loadFile(char *filename) {
    //load world
    if(strcmp(filename, "main.wld")==0) {
        loadWorldInternal(filename, loadEManager, loadWorldData);
        loadHadWorld = true;
    }
  
    //load player data of active players
    for(int i=0; i<playerCount; i++) {
        char playerFilename[50];
        playerFilename[0] = '\0';
        sprintf(playerFilename, "%lu.plr", players[i].id);
        
        if(strcmp(filename, playerFilename)==0) {
            loadPlayerInternal(filename, loadPlayers+i, loadEManager);
        }
    }
  
    return 0;
}

bool loadWorld(char *filename, EntityManager *eManager, WorldData *worldData, PlayerData *players, int playerCount) {
    //check if save file exists
    bool exists = false;
    FILE *testFile = fopen(filename, "rb");
    if(testFile) {
        fclose(testFile);
        exists = true;
    }
    if(!exists) return false;
    
    loadHadWorld = false;
    loadEManager = eManager;
    loadWorldData = worldData;
    loadPlayers = players;
    loadPlayerCount = playerCount;
    
    //extract files
    if(unzipAndLoad(filename, &loadFile, SAVE_COMMENT, ZIPHELPER_CLEANUP_FILES)!=0) {
        return false;
    }
    
    if(!loadHadWorld) {
        return false;
    }
    
    return true;
}

s8 checkFileNameForErrors(char *filename) {
    int length = strlen(filename);
    if(length < 1) return 1; // Error: Length cannot be 0.
    int i;
    bool isGood = false;
    for(i=0; i < length; ++i) { if(isalnum((int)filename[i])) isGood = true; }
    if(!isGood) return 2; // Error: Filename must contain atleast one letter or number.
    
    DIR * d;
    struct dirent * dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".msv") != NULL) { // Check if filename contains ".msv"
                char cmprFile[256];
                strncpy(cmprFile, dir->d_name, strlen(dir->d_name)-4);
                if(strncmp(filename, cmprFile, strlen(filename)) == 0) return 3; // Error: Filename cannot already exist.
            }
        }
        closedir(d);
    }
    
    return 0; // No errors found!
}
