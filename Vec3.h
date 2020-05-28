#ifndef VEC3_H
#define VEC3_H
#include <immintrin.h>

union _Vec3 {
    __m256d m;
    double arr[4];
};

typedef union _Vec3 Vec3;

Vec3* makeVec3(double, double, double);
double dot(Vec3*, Vec3*);
Vec3* sub(Vec3*, Vec3*);
void printVec3(Vec3*);
void normalize(Vec3*);
Vec3* add(Vec3*, Vec3*);
Vec3* add3(Vec3*, Vec3*, Vec3*);
void scaleVec3(Vec3*, double);
Vec3* reflectVector(Vec3*, Vec3*, double);
Vec3* refractVector(Vec3*, Vec3*, double, double);
Vec3* copyScaleVec3(Vec3*, double);
void copyVec3(Vec3*, Vec3*);
Vec3* cross(Vec3*, Vec3*);
Vec3** getOrthogonalVectors(Vec3*);
Vec3* rotate(Vec3*, Vec3*, double);
void incVec3(Vec3*, Vec3*);
void setVec3(Vec3*, double, double, double);
void setAddVec3(Vec3*, Vec3*, Vec3*);
Vec3* invert(Vec3*);

#endif