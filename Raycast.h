#ifndef RAYCAST_H
#define RAYCAST_H
#include <inttypes.h>
#include <stdio.h>
#include "Vec3.h"
#include "Sphere.h"
#include "Color.h"

struct _ThreadArgs {
    Vec3* step1;
    Vec3* step2;
    Sphere** spheres;
    int sphereLength;
    Rgb** pixels;
    uint16_t row;
};

typedef struct _ThreadArgs ThreadArgs;

void raycast(FILE*, Sphere**, int);


#endif