#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Vec3.h"

Vec3* makeVec3(double x, double y, double z) {
    Vec3* vec = malloc(sizeof(Vec3));
    vec -> x = x;
    vec -> y = y;
    vec -> z = z;
    vec -> mag2 = x * x + y * y + z * z;
    return vec;
}

Vec3 initVec3(double x, double y, double z) {
    Vec3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.mag2 = x * x + y * y + z * z;
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

void printVec3(Vec3* a) {
    printf("(%f, %f, %f)", a -> x, a -> y, a -> z);
}

void normalize(Vec3* vec) {
    double m = sqrt(vec -> mag2);
    vec -> x /= m;
    vec -> y /= m;
    vec -> z /= m;
    vec -> mag2 = 1.0;
}

void scaleVec3(Vec3* vec, double d) {
    vec -> x *= d;
    vec -> y *= d;
    vec -> z *= d;
    vec -> mag2 *= d * d;
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
    Vec3* new = makeVec3(vec -> x, vec -> y, vec -> z);
    scaleVec3(new, d);
    return new;
}

/* First parameter is destination
*/
void copyVec3(Vec3* to, Vec3* from) {
    to -> x = from -> x;
    to -> y = from -> y;
    to -> z = from -> z;
    to -> mag2 = from -> mag2;
}

Vec3* cross(Vec3* a, Vec3* b) {
    return makeVec3(a -> y * b -> z - a -> z * b -> y,
                    a -> z * b -> x - a -> x * b -> z,
                    a -> x * b -> y - a -> y * b -> x);
}

Vec3** getOrthogonalVectors(Vec3* vec) {
    Vec3 temp;
    copyVec3(&temp, vec);
    temp.x += 10; // any non-parallel vector
    temp.y += 1;
    temp.z += 1;

    Vec3* v1 = cross(vec, &temp);
    Vec3* v2 = cross(vec, v1);
    scaleVec3(v2, -1); // just to make it work

    normalize(v1);
    normalize(v2);

    Vec3** ans = malloc(sizeof(Vec3*) * 2);
    ans[1] = v1;
    ans[0] = v2;
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
    a -> x += b -> x;
    a -> y += b -> y;
    a -> z += b -> z;
    a -> mag2 = (a -> x * a -> x + a -> y * a -> y + a -> z * a -> z);
}

void setVec3(Vec3* vec, double a, double b, double c) {
    vec -> x = a;
    vec -> y = b;
    vec -> z = c;
    vec -> mag2 = a * a + b * b + c * c;
}

void setAddVec3(Vec3* a, Vec3* b, Vec3* c) {
    a -> x = b -> x + c -> x;
    a -> y = b -> y + c -> y;
    a -> z = b -> z + c -> z;
    a -> mag2 = (a -> x * a -> x + a -> y * a -> y + a -> z * a -> z);
}

void setMag(Vec3* vec) {
    double x = vec -> x;
    double y = vec -> y;
    double z = vec -> z;
    vec -> mag2 = x * x + y * y + z * z;
}

double distance2(Vec3* a, Vec3* b) {
    double x = a -> x - b -> x;
    double y = a -> y - b -> y;
    double z = a -> z - b -> z;
    return x * x + y * y + z * z;
}

int maxDimension(Vec3* v) {
    return v -> x > v -> y ? v -> x > v -> z ? 0 : 2 : v -> y > v -> z ? 1 : 2;
}

double getCoordinate(Vec3* v, int i) {
    return i == 0 ? v -> x : i == 1 ? v -> y : v -> z;
}

Vec3* invVec3(Vec3* v) {
    return makeVec3(1 / v -> x, 1 / v -> y, 1 / v -> z);
}

/**
 * Scale the components of a according to b
 */
void multVec3(Vec3* a, Vec3* b) {
    a -> x *= b -> x;
    a -> y *= b -> y;
    a -> z *= b -> z;
}