#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H
#include "../math/Transform.h"
#include "../math/Vec3.h"
#include "Ray.h"
#include "BoundingBox.h"

struct _Mesh {
    Transform* toWorld;
    int nTriangles;
    int nVert;
    int* vertInd; // of size 3 * nTriangles, TODO: Need this?
    // ith triangle has vertices at p[vertInd[3 * i]], ...
    Vec3* p; // vertices
    Vec3* n; // normal at vertex, interpolated over
    Vec3* uv; // uv coordinates of vertices
    BoundingBox* box; // in world space
    double refractivity;
    double reflectivity;
    double refractionIndex;
};

typedef struct _Mesh Mesh;

Vec3 meshIntersect(Mesh*, Ray*, double*);

#endif