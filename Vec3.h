#ifndef VEC3_H
#define VEC3_H

struct _Vec3 {
    double x;
    double y;
    double z;
};

typedef struct _Vec3 Vec3;

Vec3* makeVec3(double, double, double);
double dot(Vec3*, Vec3*);
Vec3* sub(Vec3*, Vec3*);
double mag(Vec3*);
void printVec3(Vec3*);
void normalize(Vec3*);
Vec3* add(Vec3*, Vec3*);

#endif