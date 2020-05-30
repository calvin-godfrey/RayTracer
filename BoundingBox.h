#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include "Ray.h"

struct _BoundingBox {
    Vec3* min;
    Vec3* max;
};

typedef struct _BoundingBox BoundingBox;

int intersectsBox(Ray*, BoundingBox*);

#endif