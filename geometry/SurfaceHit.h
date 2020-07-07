#ifndef SURFACEHIT_H
#define SURFACEHIT_H
#include "../math/Vec3.h"
#include "../Color.h"

struct _SurfaceHit {
    Vec3* uv; // texture coordinates
    Vec3* dpdu;
    Vec3* dpdv;
    Vec3* n; // normal
    Vec3* dndu;
    Vec3* dndv;
    double time; // hit time (from ray)
    Vec3* p; // hit location
    Rgb* color;
};

typedef struct _SurfaceHit SurfaceHit;

SurfaceHit makeSurfaceHit(double, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*, Vec3*, Rgb*);
SurfaceHit* makeEmptySurfaceHit();

#endif