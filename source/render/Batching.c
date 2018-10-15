#include "Batching.h"

#include <stdlib.h>

typedef struct {
    sf2d_vertex_pos_tex *vertices;
    int size;
    int index;
    int start_index;
    sf2d_texture *bound;
    int blend;
    u32 color;
} batch;

static batch *current_batch = NULL;

void batch_init(int size) {
    current_batch = calloc(1, sizeof(batch));
    if (!current_batch) return;
    // 3 vertices * 2 triangles per texture
    current_batch->size = size * 6 * sizeof(sf2d_vertex_pos_tex);
    current_batch->vertices = linearAlloc(current_batch->size);
    if (!current_batch->vertices) {
        free(current_batch);
    }
}

void batch_free() {
    linearFree(current_batch->vertices);
    free(current_batch);
}

//only call once per frame start (to reset indices)
void batch_reset() {
    current_batch->index = 0;
    current_batch->start_index = 0;
    current_batch->bound = NULL;
    current_batch->blend = 0;
    current_batch->color = 0;
}

//reset stored state but do not reset indices, as they can only be used once per frame
void batch_start() {
    current_batch->bound = NULL;
    current_batch->blend = 0;
    current_batch->color = 0;
}

void batch_texture(sf2d_texture *texture, int x, int y) {
    _batch_texture_part_scale_blend(texture, x, y, 0, 0, texture->width, texture->height, 1, 1, 0, 0);
}

void batch_texture_scale(sf2d_texture *texture, int x, int y, float x_scale, float y_scale) {
    _batch_texture_part_scale_blend(texture, x, y, 0, 0, texture->width, texture->height, x_scale, y_scale, 0, 0);
}

void batch_texture_part(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h) {
    _batch_texture_part_scale_blend(texture, x, y, tex_x, tex_y, tex_w, tex_h, 1, 1, 0, 0);
}

void batch_texture_part_scale(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale) {
    _batch_texture_part_scale_blend(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, 0, 0);
}

void batch_texture_blend(sf2d_texture *texture, int x, int y, u32 color) {
    _batch_texture_part_scale_blend(texture, x, y, 0, 0, texture->width, texture->height, 1, 1, 1, color);
}

void batch_texture_part_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, u32 color) {
    _batch_texture_part_scale_blend(texture, x, y, tex_x, tex_y, tex_w, tex_h, 1, 1, 1, color);
}

void batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, u32 color) {
    _batch_texture_part_scale_blend(texture, x, y, tex_x, tex_y, tex_w, tex_h, x_scale, y_scale, 1, color);
}

void _batch_texture_part_scale_blend(sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, int blend, u32 color) {
    if (current_batch == NULL) return;

    // Out of space
    if (current_batch->index >= current_batch->size) return;

    if (current_batch->bound != texture || current_batch->blend != blend || (blend && current_batch->color != color)) {
        if (current_batch->index - current_batch->start_index > 0)
            batch_flush();
        
        if(blend) {
            sf2d_bind_texture_color(texture, GPU_TEXUNIT0, color);
        } else {
            sf2d_bind_texture(texture, GPU_TEXUNIT0);
        }
        
        current_batch->start_index = current_batch->index;
        
        current_batch->bound = texture;
        current_batch->blend = blend;
        current_batch->color = color;
    }

	sf2d_vertex_pos_tex *vertices = current_batch->vertices + current_batch->index;

	vertices[0].position = (sf2d_vector_3f){(float)x,               (float)y,               SF2D_DEFAULT_DEPTH};
	vertices[1].position = (sf2d_vector_3f){(float)x,               (float)y+tex_h*y_scale, SF2D_DEFAULT_DEPTH};
	vertices[2].position = (sf2d_vector_3f){(float)x+tex_w*x_scale, (float)y,               SF2D_DEFAULT_DEPTH};

	vertices[3].position = (sf2d_vector_3f){(float)x+tex_w*x_scale, (float)y,               SF2D_DEFAULT_DEPTH};
	vertices[4].position = (sf2d_vector_3f){(float)x,               (float)y+tex_h*y_scale, SF2D_DEFAULT_DEPTH};
	vertices[5].position = (sf2d_vector_3f){(float)x+tex_w*x_scale, (float)y+tex_h*y_scale, SF2D_DEFAULT_DEPTH};

	float u0 = tex_x/(float)texture->tex.width;
	float v0 = tex_y/(float)texture->tex.height;
	float u1 = (tex_x+tex_w)/(float)texture->tex.width;
	float v1 = (tex_y+tex_h)/(float)texture->tex.height;

	vertices[0].texcoord = (sf2d_vector_2f){u0, v0};
	vertices[1].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[2].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[3].texcoord = (sf2d_vector_2f){u1, v0};
	vertices[4].texcoord = (sf2d_vector_2f){u0, v1};
	vertices[5].texcoord = (sf2d_vector_2f){u1, v1};

	current_batch->index += 6;
}

void batch_end() {
    if (current_batch == NULL) return;

    if (current_batch->index - current_batch->start_index > 0)
        batch_flush();
}

void batch_flush() {
    if (current_batch == NULL) return;

	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, current_batch->vertices, sizeof(sf2d_vertex_pos_tex), 2, 0x10);

	C3D_DrawArrays(GPU_TRIANGLES, current_batch->start_index, current_batch->index - current_batch->start_index);
    
    current_batch->start_index = current_batch->index;
}
