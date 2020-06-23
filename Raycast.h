#ifndef RAYCAST_H
#define RAYCAST_H
#include <inttypes.h>
#include <stdio.h>
#include "math/Vec3.h"
#include "geometry/Sphere.h"
#include "Color.h"
#include "ObjectWrapper.h"

struct _ThreadArgs {
    Vec3* step1;
    Vec3* step2;
    Wrapper* spheres;
    unsigned char* pixels;
    uint16_t row;
};

typedef struct _ThreadArgs ThreadArgs;
void raycast(FILE*, Wrapper*);

#endif