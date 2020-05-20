#include <stdio.h>
#include <stdlib.h>
#include "Texture.h"
#include "Consts.h"

static int isValidTga(FILE*);

Texture* makeTexture(char* fileName) {
    uint16_t width, height;
    int widthOffset = 12;
    int heightOffset = 14;
    Texture* t = malloc(sizeof(Texture));
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error opening texture file %s\n", fileName);
        return NULL;
    }
    t -> file = file;
    t -> type = TGA; // TODO: Support multiple file types
    // in TGA, bytes 12-13 are width, 14-15 are height
    fseek(file, widthOffset, SEEK_SET); // SEEK_SET is start of file
    if (fread(&width, sizeof(uint16_t), 1, file) != 1) {
        printf("Error getting width from texture file %s\n", fileName);
        return NULL;
    }
    fseek(file, heightOffset, SEEK_SET);
    if (fread(&height, sizeof(uint16_t), 1, file) != 1) {
        printf("Error getting height from texture file %s\n", fileName);
        return NULL;
    }
    if (!isValidTga(file)) {
        printf("Bad TGA %s\n", fileName);
        return NULL;
    }
    t -> width = width;
    t -> height = height;
    t -> headerSize = 18; // For now, this can only support fixed-size TGA headers

    char orientation;
    fseek(file, 17, SEEK_SET);
    fread(&orientation, sizeof(char), 1, file);
    t -> flipX = (orientation & (1 << 4)) > 0 ? -1 : 1;
    t -> flipY = (orientation & (1 << 5)) > 0 ? -1 : 1;
    return t;
}

void freeTexture(Texture* t) {
    fclose(t -> file);
}

/**
 * Returns the pixel data at the location specified by x and y, which are assumed
 * to both be in [0, 1). x,y=0,0 is bottom left corner, while x,y=1,1
 * represents the top right corner.
 */
Rgb* getPixel(Texture* t, double x, double y) {
    unsigned char red, green, blue;
    x = t -> flipX == -1 ? 1 - x : x;
    y = t -> flipY == -1 ? 1 - y : y;

    uint16_t xCoord = (uint16_t)(t -> width * x);
    uint16_t yCoord = (uint16_t)(t -> height * y);
    uint32_t offset = yCoord * t -> width + xCoord;
    fseek(t -> file, (uint32_t)(t -> headerSize) + offset * 3, SEEK_SET); // * 3 because 3 bytes per pixel
    fread(&blue, sizeof(char), 1, t -> file);
    fseek(t -> file, (uint32_t)(t -> headerSize) + offset * 3 + 1, SEEK_SET);
    fread(&green, sizeof(char), 1, t -> file);
    fseek(t -> file, (uint32_t)(t -> headerSize) + offset * 3 + 2, SEEK_SET);
    fread(&red, sizeof(char), 1, t -> file);

    Rgb* color = makeRgb(red, green, blue);
    return color;
}

static int isValidTga(FILE* file) {
    char idField;
    char mapType;
    char imageType;
    char bitDepth;
    fseek(file, 0, SEEK_SET);
    fread(&idField, sizeof(char), 1, file);
    if (idField != 0) {
        printf("TGA file ID field bad\n");
        return 0;
    }

    fseek(file, 1, SEEK_SET);
    fread(&mapType, sizeof(char), 1, file);
    if (mapType != 0) {
        printf("TGA file Map type bad\n");
        return 0;
    }

    fseek(file, 2, SEEK_SET);
    fread(&imageType, sizeof(char), 1, file);
    if (imageType != 2) { // uncompressed true-color image
        printf("TGA file image type bad\n");
        return 0;
    }

    fseek(file, 16, SEEK_SET);
    fread(&bitDepth, sizeof(char), 1, file);
    if (bitDepth != 24) {
        printf("TGA file has wrong bit depth (24 bits per pixel expected)\n");
        return 0;
    }
    return 1;
}