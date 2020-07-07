#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H
#include "../math/Transform.h"
#include "../math/Vec3.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "SurfaceHit.h"
#include "Material.h"

struct _Mesh {
    Transform* toWorld;
    int nTriangles;
    int nVert;
    int* pInd; // of size 3 * nTriangles, TODO: Need this?
    // ith triangle has vertices at p[vertInd[3 * i]], ...
    int *nInd;
    int* uvInd;
    Vec3* p; // vertices
    Vec3* n; // normal at vertex, interpolated over
    Vec3* uv; // uv coordinates of vertices
    BoundingBox* box; // in world space
    Material** mats;
    int* matInd;
    double refractivity;
    double reflectivity;
    double refractionIndex;
};

typedef struct _Mesh Mesh;

SurfaceHit* meshIntersect(Mesh*, Ray*, double*);
double intersectsTriangle(Mesh*, Ray*, int*, int*, int*, double, double, SurfaceHit*);

#endif