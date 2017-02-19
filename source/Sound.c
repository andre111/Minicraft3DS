#include "Sound.h"

void loadSound(Sound * snd, char * filename){
    FILE *file = fopen(filename, "rb");
    if(file != NULL){
        fseek(file, 0, SEEK_END);
        snd->size = ftell(file);
        fseek(file, 0, SEEK_SET);
        snd->buffer = linearAlloc(snd->size);
        fread(snd->buffer, 1, snd->size, file);
    }
    fclose(file);
}

void playSound(Sound snd){
    csndPlaySound(8, SOUND_FORMAT_16BIT | SOUND_ONE_SHOT, 44100, 1, 0, snd.buffer, snd.buffer, snd.size);
}

void playMusic(Sound snd){
    csndPlaySound(10, SOUND_FORMAT_16BIT | SOUND_REPEAT, 44100, 1, 0, snd.buffer, snd.buffer, snd.size);
}

void stopMusic() {
	CSND_SetPlayState(8, 0);
	CSND_SetPlayState(10, 0);
    csndExecCmds(true);
}

void updateMusic(int lvl, int time) {
	switch(lvl) {
	case 0:
		playMusic(music_floor0);
		break;
	case 1:
		if(time>6000 && time<19000) playMusic(music_floor1);
		else playMusic(music_floor1_night);
		break;
	case 2:
	case 3:
		playMusic(music_floor23);
		break;
	case 4:
	case 5: //TODO - dungeon needs own music
		playMusic(music_floor4);
		break;
	}
}

void loadSounds() {
	loadSound(&snd_playerHurt, "resources/playerhurt.raw");
	loadSound(&snd_playerDeath, "resources/playerdeath.raw");
	loadSound(&snd_monsterHurt, "resources/monsterhurt.raw");
	loadSound(&snd_test, "resources/test.raw");
	loadSound(&snd_pickup, "resources/pickup.raw");
	loadSound(&snd_bossdeath, "resources/bossdeath.raw");
	loadSound(&snd_craft, "resources/craft.raw");
	
	loadSound(&music_menu, "resources/music/menu.raw");
	loadSound(&music_floor0, "resources/music/floor0.raw");
	loadSound(&music_floor1, "resources/music/floor1.raw");
	loadSound(&music_floor1_night, "resources/music/floor1_night.raw");
	loadSound(&music_floor23, "resources/music/floor2_3.raw");
	loadSound(&music_floor4, "resources/music/floor4.raw");
}

void freeSounds(){
    linearFree(snd_playerHurt.buffer);
    linearFree(snd_playerDeath.buffer);
    linearFree(snd_monsterHurt.buffer);
    linearFree(snd_test.buffer);
    linearFree(snd_pickup.buffer);
    linearFree(snd_bossdeath.buffer);
    linearFree(snd_craft.buffer);
	
	linearFree(music_menu.buffer);
	linearFree(music_floor0.buffer);
	linearFree(music_floor1.buffer);
	linearFree(music_floor1_night.buffer);
	linearFree(music_floor23.buffer);
	linearFree(music_floor4.buffer);
}
