#ifndef RAY_H
#define RAY_H
#include "Vec3.h"

struct _Ray {
    Vec3* from;
    Vec3* to;
    Vec3* dir;
};

typedef struct _Ray Ray;

Ray* makeRay(Vec3*, Vec3*);
void freeRay(Ray*);
void printRay(Ray*);
Vec3* getPoint(Ray*, double);
int onLine(Ray*, double);


#endif