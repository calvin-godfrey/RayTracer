#include <stdlib.h>
#include "SurfaceHit.h"

/**
 * time, point, dpdu, dpdv, normal, dndu, dndv, uv
 */
SurfaceHit makeSurfaceHit(double time, Vec3* point, Vec3* dpdu, Vec3* dpdv, Vec3* normal, Vec3* dndu, Vec3* dndv, Vec3* uv) {
    SurfaceHit s;
    s.time = time;
    s.p = point;
    s.dpdu = dpdu;
    s.dpdv = dpdv;
    s.n = normal;
    s.dndu = dndu;
    s.dndv = dndv;
    s.uv = uv;
    return s;
}