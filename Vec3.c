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

/**
 * Reflects the Vector vec across the axis determined by the base vector. Assumes that the
 * two vectors passed are both unit vectors.
 */
Vec3* reflectVector(Vec3* base, Vec3* vec) {
    // dot(base, vec) * base is the projection of vec onto base, so that
    // `vec - dot(base, vec) * base` is perpendicular to base. To make it a proper reflection,
    // we simply want to subract the projection again, giving the formula used here.
    double projectionFactor = 2 * dot(base, vec);
    Vec3* baseCopy = copyScaleVec3(base, projectionFactor);
    Vec3* ans = sub(vec, baseCopy); // vec - 2 * dot(base, vec) * base;
    free(baseCopy);
    return ans;
}

Vec3* refractVector(Vec3* dir, Vec3* normal, double factor) {
    double cosi = -dot(normal, dir);
    double sini2 = factor * factor * (1 - cosi * cosi);
    if (sini2 > 1.0) return NULL;
    double cost = sqrt(1.0 - sini2);
    Vec3* scaledDir = copyScaleVec3(dir, factor);
    Vec3* scaledNormal = copyScaleVec3(normal, factor * cosi - cost);
    Vec3* ans = add(scaledDir, scaledNormal);
    free(scaledDir);
    free(scaledNormal);
    return ans;
}

Vec3* copyScaleVec3(Vec3* vec, double d) {
    Vec3* new = makeVec3(vec -> x, vec -> y, vec -> z);
    scaleVec3(new, d);
    return new;
}

void copyVec3(Vec3* from, Vec3* to) {
    to -> x = from -> x;
    to -> y = from -> y;
    to -> z = from -> z;
}