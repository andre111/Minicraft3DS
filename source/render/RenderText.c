#include "RenderText.h"

#include "Batching.h"
#include "../Render.h"

static char * fontChars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ      0123456789.,!?'\"-+=/\\%()<>:;     ";

void renderText(char * msg, u32 x, u32 y) {
    batch_start();
    int i = 0;
    x -= offsetX << 1;
    y -= offsetY << 1;
    for (i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char) msg[i])) - fontChars;
        int iy = ix >> 5;
        if (ix >= 0) {
            batch_texture_part(font, x + i * 12, y, (ix & 31) * 12,
                    16 + (iy * 12), 12, 12);
        }
    }
    batch_end();
}

void renderTextSized(char * msg, u32 x, u32 y, float size) {
    batch_start();
    int i = 0;
    for (i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char) msg[i])) - fontChars;
        int iy = ix >> 5;
        if (ix >= 0) {
            batch_texture_part_scale(font, (x + i * 8) * size, y,
                    (ix & 31) << 3, iy << 3, 8, 8, size, size);
        }
    }
    batch_end();
}

void renderTextColor(char * msg, u32 x, u32 y, u32 color) {
    batch_start();
    int i = 0;
    x -= offsetX << 1;
    y -= offsetY << 1;
    for (i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char) msg[i])) - fontChars;
        int iy = ix >> 5;
        if (ix >= 0) {
            batch_texture_part_blend(font, x + i * 12, y, (ix & 31) * 12,
                    16 + (iy * 12), 12, 12, color);
        }
    }
    batch_end();
}

void renderTextColorSized(char * msg, int x, int y, float size, u32 color) {
    batch_start();
    int i;
    for (i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char) msg[i])) - fontChars;
        int iy = ix >> 5;
        if (ix >= 0)
            batch_texture_part_scale_blend(font, (x + i * 8) * size,
                    y * size, (ix & 31) << 3, iy << 3, 8, 8, size, size, color);
    }
    batch_end();
}

// Changes text color after the first space
void renderTextColorSpecial(char * msg, u32 x, u32 y, u32 color, u32 color2) {
    batch_start();
    int i = 0;
    x -= offsetX << 1;
    y -= offsetY << 1;
    bool sOver = false;
    for (i = 0; i < strlen(msg); ++i) {
        int ix = strchr(fontChars, toupper((unsigned char) msg[i])) - fontChars;
        if (msg[i] == ' ')
            sOver = true;
        int iy = ix >> 5;
        if (ix >= 0) {
            if (sOver)
                batch_texture_part_blend(font, x + i * 12, y,
                        (ix & 31) * 12, 16 + (iy * 12), 12, 12, color2);
            else
                batch_texture_part_blend(font, x + i * 12, y,
                        (ix & 31) * 12, 16 + (iy * 12), 12, 12, color);
        }
    }
    batch_end();
}
