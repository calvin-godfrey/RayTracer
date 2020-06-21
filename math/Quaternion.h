#ifndef QUATERNION_H
#define QUATERNION_H
#include "Vec3.h"

struct _Quaternion {
    double w, x, y, z;
    Vec3* vec;
};


// each in radians
struct _Angles {
    double x, y, z;
};

typedef struct _Quaternion Quaternion;
typedef struct _Angles Angles;

Quaternion* anglesToQuaternion(Angles*);
Angles* quaternionToAngle(Quaternion*);
Quaternion* makeQuaternion(double, double, double, double);
Angles* makeAngles(double, double, double);
Vec3* multiply(Quaternion*, Vec3*);
Quaternion* copyScaleQ(Quaternion*, double);
void freeQuaternion(Quaternion*);

#endif