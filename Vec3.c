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

Vec3* cross(Vec3* a, Vec3* b) {
    return makeVec3(a -> y * b -> z - a -> z * b -> y,
                    a -> z * b -> x - a -> x * b -> z,
                    a -> x * b -> y - a -> y * b -> x);
}

Vec3** getOrthogonalVectors(Vec3* vec) {
    Vec3* temp = makeVec3(0, 0, 0);
    copyVec3(vec, temp);
    temp -> z += 1; // any non-parallel vector

    Vec3* v1 = cross(vec, temp);
    Vec3* v2 = cross(vec, v1);
    scaleVec3(v2, -1); // just to make it work

    normalize(v1);
    normalize(v2);

    Vec3** ans = malloc(sizeof(Vec3*) * 2);
    ans[1] = v1;
    ans[0] = v2;
    free(temp);
    return ans;
}

Vec3* rotate(Vec3* vec, Vec3* axis, double angle) {
    double c = cos(angle);
    double s = sin(-angle);
    double d = dot(axis, vec);
    Vec3* crossed = cross(axis, vec);
    scaleVec3(crossed, s);
    Vec3* scaled = copyScaleVec3(vec, c);
    Vec3* last = copyScaleVec3(axis, d * (1 - c));
    Vec3* ans = add3(crossed, scaled, last);
    free(crossed);
    free(scaled);
    free(last);
    return ans;
}

Vec3* add3(Vec3* a, Vec3* b, Vec3* c) {
    Vec3* temp = add(a, b);
    Vec3* ans = add(temp, c);
    free(temp);
    return ans;
}