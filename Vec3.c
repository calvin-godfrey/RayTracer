#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Vec3.h"

Vec3* makeVec3(double x, double y, double z) {
    Vec3* vec = malloc(sizeof(Vec3));
    vec -> x = x;
    vec -> y = y;
    vec -> z = z;
    return vec;
}

double dot(Vec3* a, Vec3* b) {
    return a -> x * b -> x + a -> y * b -> y + a -> z * b -> z;
}

Vec3* sub(Vec3* a, Vec3* b) {
    return makeVec3(a -> x - b -> x, a -> y - b -> y, a -> z - b -> z);
}

Vec3* add(Vec3* a, Vec3* b) {
    return makeVec3(a -> x + b -> x, a -> y + b -> y, a -> z + b -> z);
}

double mag(Vec3* a) {
    return a -> x * a -> x + a -> y * a -> y + a -> z * a -> z;
}

void printVec3(Vec3* a) {
    printf("(%f, %f, %f)", a -> x, a -> y, a -> z);
}

void normalize(Vec3* vec) {
    double m = sqrt(mag(vec));
    vec -> x /= m;
    vec -> y /= m;
    vec -> z /= m;
}

void scaleVec3(Vec3* vec, double d) {
    vec -> x *= d;
    vec -> y *= d;
    vec -> z *= d;
}