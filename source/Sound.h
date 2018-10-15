#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <3ds.h>

//Note: The lower channels (<8 ?) seem to not work?
#define SOUND_CHANNEL_MIN 16
#define SOUND_CHANNEL_MAX 24
#define MUSIC_CHANNEL 10

#define MUSIC_STREAM_BUFFER_SIZE (65536*8)

typedef struct {
    u32 size;
    u8 * buffer;
} Sound;

typedef struct {
    u32 size;
    u32 pos;
    char *filename;
} Music;

void loadSound(Sound * snd, char * filename);
void playSound(Sound snd);
void playSoundPositioned(Sound snd, s8 level, int x, int y);
void setListenerPosition(s8 level, int x, int y);

void loadMusic(Music *music, char * filename);
void playMusic(Music *music);
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

Music music_menu;
Music music_floor0;
Music music_floor1;
Music music_floor1_night;
Music music_floor23;
Music music_floor4;