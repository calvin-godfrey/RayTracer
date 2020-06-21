#ifndef RAY_H
#define RAY_H
#include "../math/Vec3.h"

struct _Ray {
    Vec3* from;
    Vec3* dir;
};

typedef struct _Ray Ray;

Ray* makeRay(Vec3*, Vec3*);
void freeRay(Ray*);
void printRay(Ray*);
Vec3* getPoint(Ray*, double);
Ray* makeRayPointDir(Vec3*, Vec3*);

#endif