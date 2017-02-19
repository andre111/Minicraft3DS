#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <3ds.h>


typedef struct {
    u32 size;
    u8 * buffer;
} Sound;

void loadSound(Sound * snd, char * filename);
void playSound(Sound snd);

void playMusic(Sound snd);
void updateMusic(int lvl, int time);
void stopMusic();

void loadSounds();
void freeSounds();

Sound snd_playerHurt;
Sound snd_playerDeath;
Sound snd_monsterHurt;
Sound snd_test;
Sound snd_pickup;
Sound snd_bossdeath;
Sound snd_craft;

Sound music_menu;
Sound music_floor0;
Sound music_floor1;
Sound music_floor1_night;
Sound music_floor23;
Sound music_floor4;