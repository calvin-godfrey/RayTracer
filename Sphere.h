#ifndef SPHERE_H
#define SPHERE_H
#include "Color.h"
#include "Vec3.h"
#include "Ray.h"
#include "Texture.h"
#include "Quaternion.h"
#include "BoundingBox.h"

struct _Sphere {
    Vec3* center;
    double radius;
    Rgb* color;
    double reflectivity;
    double refractivity;
    Texture* texture;
    Quaternion* rotation;
    double refractionIndex;
    BoundingBox* box;
};

typedef struct _Sphere Sphere;

Sphere* makeSphere(Vec3*, double, Rgb*, Texture*, double, double, double);
Sphere* makeSphereRotation(Vec3*, double, Rgb*, Texture*, double, double, double, double, double, double);
void freeSphere(Sphere*);
Vec3* sphereIntersect(Sphere*, Ray*, double*);
Rgb* getPixelData(Sphere*, Vec3*);


#endif