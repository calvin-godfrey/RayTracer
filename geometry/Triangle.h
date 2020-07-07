#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "TriangleMesh.h"
#include "../math/Transform.h"
#include "BoundingBox.h"
#include "SurfaceHit.h"

struct _Triangle {
    Mesh* mesh;
    int* vert;
    int* norm;
    int* uv;
};

typedef struct _Triangle Triangle;

#endif