#include <math.h>
#include <stdlib.h>
#include <stdio.h>
// #include <immintrin.h>
#include "BoundingBox.h"

int intersectsBox(Ray* ray, BoundingBox* box) {
    Vec3* inv = invVec3(ray -> dir);

    double tmax = inv -> x * ((inv -> x < 0 ? box -> min -> x : box -> max -> x) - ray -> from -> x);
    double tmin = inv -> x * ((inv -> x < 0 ? box -> max -> x : box -> min -> x) - ray -> from -> x);

    double tymax = inv -> y * ((inv -> y < 0 ? box -> min -> y : box -> max -> y) - ray -> from -> y);
    double tymin = inv -> y * ((inv -> y < 0 ? box -> max -> y : box -> min -> y) - ray -> from -> y);

    if ((tmin > tymax) || (tymin > tmax)) { free(inv); return 0;}

    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    double tzmax = inv -> z * ((inv -> z < 0 ? box -> min -> z : box -> max -> z) - ray -> from -> z);
    double tzmin = inv -> z * ((inv -> z < 0 ? box -> max -> z : box -> min -> z) - ray -> from -> z);

    if ((tmin > tzmax) || (tzmin > tmax)) { free(inv); return 0;}

    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    free(inv);

    double t = tmin;
    if (t < 0) {
        t = tmax;
        if (t < 0) return 0;
    }

    return 1;

    // __m256d min = _mm256_load_pd((double*) box -> min);
    // __m256d max = _mm256_load_pd((double*) box -> max);
    // __m256d from = _mm256_load_pd((double*) ray -> from);
    // Vec3* i = invert(ray -> dir);
    // __m256d inv = _mm256_load_pd((double*) i);

    // __m256d diffmin = _mm256_sub_pd(min, from);
    // __m256d diffmax = _mm256_sub_pd(max, from);

    // __m256d finalmin = _mm256_mul_pd(diffmin, inv);
    // __m256d finalmax = _mm256_mul_pd(diffmax, inv);

    // __m256d tmin = _mm256_min_pd(finalmin, finalmax);
    // __m256d tmax = _mm256_max_pd(finalmin, finalmax);

    // double ansmin[4];
    // double ansmax[4];

    // _mm256_store_pd(ansmin, tmax);
    // _mm256_store_pd(ansmax, tmin);

    // double mi = fmin(ansmax[0], fmin(ansmax[1], ansmax[2]));
    // double ma = fmax(ansmin[0], fmax(ansmin[1], ansmin[2]));

    // return ma <= mi ? 1 : 0;


    // double t1 = (box -> min -> x - ray -> from -> x) * inv -> x;
    // double t2 = (box -> max -> x - ray -> from -> x) * inv -> x;
    // double tmin = fmin(t1, t2);
    // double tmax = fmax(t1, t2);

    // t1 = (box -> min -> y - ray -> from -> y) * inv -> y;
    // t2 = (box -> max -> y - ray -> from -> y) * inv -> y;
    // tmin = fmax(tmin, fmin(fmin(t1, t2), tmax));
    // tmax = fmin(tmax, fmax(fmax(t1, t2), tmin));

    // t1 = (box -> min -> z - ray -> from -> z) * inv -> z;
    // t2 = (box -> max -> z - ray -> from -> z) * inv -> z;
    // tmin = fmax(tmin, fmin(fmin(t1, t2), tmax));
    // tmax = fmin(tmax, fmax(fmax(t1, t2), tmin));

    // return tmax > fmax(tmin, 0.0) ? 1 : 0;
}

/**
 * Returns the smallest box containing both the box
 * and point passed
 */
BoundingBox Union(BoundingBox* box, Vec3* v) {
    BoundingBox ans;
    ans.min = makeVec3(0, 0, 0);
    ans.max = makeVec3(0, 0, 0);
    ans.min -> x = fmin(box -> min -> x, v -> x);
    ans.min -> y = fmin(box -> min -> y, v -> y);
    ans.min -> z = fmin(box -> min -> z, v -> z);

    ans.max -> x = fmax(box -> max -> x, v -> x);
    ans.max -> y = fmax(box -> max -> y, v -> y);
    ans.max -> z = fmax(box -> max -> z, v -> z);
    return ans;
}

BoundingBox* makeBoundingBox(Vec3* min, Vec3* max) {
    BoundingBox* box = malloc(sizeof(BoundingBox));
    box -> min = min;
    box -> max = max;
    return box;
}