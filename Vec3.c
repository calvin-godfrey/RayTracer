#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Vec3.h"

static Vec3* makeVec3M(__m256d m) {
    Vec3* vec = malloc(sizeof(Vec3));
    vec -> m = m;
    return vec;
}

Vec3* makeVec3(double x, double y, double z) {
    Vec3* vec = malloc(sizeof(Vec3));
    vec -> arr[0] = x;
    vec -> arr[1] = y;
    vec -> arr[2] = z;
    vec -> arr[3] = x * x + y * y + z * z;
    return vec;
}

double dot(Vec3* a, Vec3* b) {
    double ax = a -> arr[0];
    double ay = a -> arr[1];
    double az = a -> arr[2];
    double bx = b -> arr[0];
    double by = b -> arr[1];
    double bz = b -> arr[2];
    return ax * bx + ay * by + az * bz;
    // Vec3 temp;
    // temp.m = _mm256_dp_pd(a, b, 0b00011111);
    // return temp.arr[0];
}

Vec3* sub(Vec3* a, Vec3* b) {
    __m256d s = _mm256_sub_pd(a -> m, b -> m);
    Vec3* ans = makeVec3M(s);
    printVec3(ans);
    return ans;
}

Vec3* add(Vec3* a, Vec3* b) {
    __m256d s = _mm256_add_pd(a -> m, b -> m);
    Vec3* ans = makeVec3M(s);
    printVec3(ans);
    return ans;
}

void printVec3(Vec3* a) {
    printf("(%f, %f, %f)", a -> arr[0], a -> arr[1], a -> arr[2]);
}

void normalize(Vec3* vec) {
    double m = 1/sqrt(vec -> arr[3]);
    vec -> arr[0] = vec -> arr[0] * m;
    vec -> arr[1] = vec -> arr[1] * m;
    vec -> arr[2] = vec -> arr[2] * m;
    vec -> arr[3] = 1.0;
}

void scaleVec3(Vec3* vec, double d) {
    vec -> arr[0] = vec -> arr[0] * d;
    vec -> arr[1] = vec -> arr[1] * d;
    vec -> arr[2] = vec -> arr[2] * d;
    vec -> arr[3] = vec -> arr[3] * d * d;
}

/**
 * Reflects the Vector vec across the axis determined by the base vector. Assumes that the
 * two vectors passed are both unit vectors.
 */
Vec3* reflectVector(Vec3* base, Vec3* vec, double dotProd) {
    // dot(base, vec) * base is the projection of vec onto base, so that
    // `vec - dot(base, vec) * base` is perpendicular to base. To make it a proper reflection,
    // we simply want to subract the projection again, giving the formula used here.
    double projectionFactor = 2 * dotProd;
    Vec3* baseCopy = copyScaleVec3(base, projectionFactor);
    Vec3* ans = sub(vec, baseCopy); // vec - 2 * dot(base, vec) * base;
    free(baseCopy);
    return ans;
}

Vec3* refractVector(Vec3* dir, Vec3* normal, double factor, double cosi) {
    double sini2 = factor * factor * (1 - cosi * cosi);
    if (sini2 > 1.0) return NULL;
    double cost = sqrt(1.0 - sini2);
    Vec3* scaledDir = copyScaleVec3(dir, factor);
    Vec3* scaledNormal = copyScaleVec3(normal, factor * cosi - cost);
    incVec3(scaledDir, scaledNormal);
    free(scaledNormal);
    return scaledDir;
}

Vec3* copyScaleVec3(Vec3* vec, double d) {
    Vec3* new = makeVec3(vec -> arr[0], vec -> arr[1], vec -> arr[2]);
    scaleVec3(new, d);
    return new;
}

void copyVec3(Vec3* from, Vec3* to) {
    to -> arr[0] = from -> arr[0];
    to -> arr[1] = from -> arr[1];
    to -> arr[2] = from -> arr[2];
}

Vec3* cross(Vec3* a, Vec3* b) {
    return makeVec3(a -> arr[1] * b -> arr[2] - a -> arr[2] * b -> arr[1],
                    a -> arr[2] * b -> arr[0] - a -> arr[0] * b -> arr[2],
                    a -> arr[0] * b -> arr[1] - a -> arr[1] * b -> arr[0]);
}

Vec3** getOrthogonalVectors(Vec3* vec) {
    Vec3* temp = makeVec3(0, 0, 0);
    copyVec3(vec, temp);
    temp -> arr[3] += 1; // any non-parallel vector

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

void incVec3(Vec3* a, Vec3* b) {
    a -> arr[0] = a -> arr[0] + b -> arr[0];
    a -> arr[1] = a -> arr[1] + b -> arr[1];
    a -> arr[2] = a -> arr[2] + b -> arr[2];
    a -> arr[3] = dot(a, a);
}

void setVec3(Vec3* vec, double a, double b, double c) {
    vec -> arr[0] = a;
    vec -> arr[1] = b;
    vec -> arr[2] = c;
    vec -> arr[3] = a * a + b * b + c * c;
}

void setAddVec3(Vec3* a, Vec3* b, Vec3* c) {
    a -> arr[0] = b -> arr[0] + c -> arr[0];
    a -> arr[1] = b -> arr[1] + c -> arr[1];
    a -> arr[2] = b -> arr[2] + c -> arr[2];
    a -> arr[3] = (a -> arr[0] * a -> arr[0] + a -> arr[1] * a -> arr[1] + a -> arr[2] * a -> arr[2]);
}

Vec3* invert(Vec3* a) {
    return makeVec3(1 / a -> arr[0], 1 / a -> arr[1], 1 / a -> arr[2]);
}