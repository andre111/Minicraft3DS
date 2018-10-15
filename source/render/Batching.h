#pragma once

#include <sf2d.h>

void batch_init(int size);
void batch_free();

//only call once per frame start (to reset indices)
void batch_reset();

void batch_start();

void batch_texture(sf2d_texture *texture, int x, int y);
void batch_texture_scale(sf2d_texture *texture, int x, int y, float x_scale, float y_scale);
void batch_texture_part(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h);
void batch_texture_part_scale(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale);
void batch_texture_blend(sf2d_texture *texture, int x, int y, u32 color);
void batch_texture_part_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, u32 color);
void batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, u32 color);

void _batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, int blend, u32 color);

void batch_end();
void batch_flush();
