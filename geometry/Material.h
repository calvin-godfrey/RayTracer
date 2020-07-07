#ifndef MATERIAL_H
#define MATERIAL_H
#include "../Texture.h"

struct _Material {
    Texture* texture;
    Texture* normal;
    char* name;
};

typedef struct _Material Material;

#endif