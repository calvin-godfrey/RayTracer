#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdio.h>
#include <inttypes.h>
#include "Color.h"

struct _Texture {
    unsigned char* bytes;
    uint16_t width;
    uint16_t height;
    char* type;
    uint16_t headerSize;
    // My code for getting the pixel data at a specific location assumes that (0, 0) is the bottom left corner,
    // but the TGA format supports that being any potential corner, so these two variables account for that.
    char flipX;
    char flipY;
};

typedef struct _Texture Texture;

Texture* makeTexture(char*);
void freeTexture(Texture*);
Rgb* getPixel(Texture*, double, double);


#endif