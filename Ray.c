#include <stdlib.h>
#include <stdio.h>
#include "Ray.h"

Ray* makeRay(Vec3* from, Vec3* to) {
    Ray* ray = malloc(sizeof(Ray));
    Vec3* newTo = sub(to, from);
    normalize(newTo);
    ray -> from = from;
    ray -> to = to;
    ray -> dir = newTo;
    ray -> pointDir = 0;
    return ray;
}

void freeRay(Ray* ray) {
    if (ray -> pointDir == 1) free(ray -> to);
    free(ray -> dir);
}

void printRay(Ray* ray) {
    printVec3(ray -> from);
    printf(" -> ");
    printVec3(ray -> to);
    printf("\n");
}

Vec3* getPoint(Ray* ray, double time) {
    double x = ray -> from -> x + (ray -> dir -> x) * time;
    double y = ray -> from -> y + (ray -> dir -> y) * time;
    double z = ray -> from -> z + (ray -> dir -> z) * time;

    Vec3* ans = malloc(sizeof(Vec3));
    ans -> x = x;
    ans -> y = y;
    ans -> z = z;
    return ans;
}

int onLine(Ray* ray, double d) {
    double toD = (ray -> to -> x - ray -> from -> x) / (ray -> dir -> x);
    return toD >= d ? 1 : 0;
}

Ray* makeRayPointDir(Vec3* point, Vec3* dir) {
    Vec3* to = add(point, dir);
    Ray* ray = makeRay(point, to);
    ray -> pointDir = 1;
    return ray;
}