#ifndef SPHERE_H
#define SPHERE_H
#include "Color.h"
#include "Vec3.h"
#include "Ray.h"

struct _Sphere {
    Vec3* center;
    double radius;
    Rgb* color;
};

typedef struct _Sphere Sphere;

Sphere* makeSphere(Vec3*, double, Rgb*);
void freeSphere(Sphere*);
Vec3* sphereIntersect(Sphere*, Ray*, double*);


#endif