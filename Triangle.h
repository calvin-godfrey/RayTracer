#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "TriangleMesh.h"
#include "../math/Transform.h"
#include "BoundingBox.h"
#include "SurfaceHit.h"

struct _Triangle {
    Mesh* mesh;
    int* vert;
};

typedef struct _Triangle Triangle;

int intersectTriangle(Ray*, Triangle*, double, double, SurfaceHit*);

#endif