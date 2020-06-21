#ifndef SPHERE_H
#define SPHERE_H
#include "../Color.h"
#include "../math/Vec3.h"
#include "Ray.h"
#include "../Texture.h"
#include "../math/Quaternion.h"

struct _Sphere {
    Vec3* center;
    double radius;
    Rgb* color;
    double reflectivity;
    double refractivity;
    Texture* texture;
    Texture* normalMap;
    Quaternion* rotation;
    double refractionIndex;
};

typedef struct _Sphere Sphere;

Sphere* makeSphere(Vec3*, double, Rgb*, Texture*, double, double, double);
Sphere* makeSphereRotation(Vec3*, double, Rgb*, Texture*, Texture*, double, double, double, double, double, double);
void freeSphere(Sphere*);
Vec3 sphereIntersect(Sphere*, Ray*, double*);
Rgb* getSpherePixel(Sphere*, Vec3*);
void adjustSphereNormal(Sphere*, Vec3*, Vec3*);
Vec3* getSphereTangent(Sphere*, Vec3*);

#endif