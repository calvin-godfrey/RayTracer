#ifndef SURFACEHIT_H
#define SURFACEHIT_H
#include "../math/Vec3.h"

struct _SurfaceHit {
    Vec3* uv;
    Vec3* dpdu;
    Vec3* dpdv;
    Vec3* n;
    Vec3* dndu;
    Vec3* dndv;
    double time;
    Vec3* p;
};

typedef struct _SurfaceHit SurfaceHit;

SurfaceHit makeSurfaceHit(double, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*);

#endif