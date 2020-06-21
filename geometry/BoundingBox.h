#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include "Ray.h"
#include "../math/Vec3.h"

struct _BoundingBox {
    Vec3* min;
    Vec3* max;
};

typedef struct _BoundingBox BoundingBox;

BoundingBox* makeBoundingBox(Vec3*, Vec3*);
int intersectsBox(Ray*, BoundingBox*);
BoundingBox Union(BoundingBox*, Vec3*);

#endif