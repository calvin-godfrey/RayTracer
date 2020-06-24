#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Triangle.h"
#include "../math/Matrix4.h"
#include "../Consts.h"

static BoundingBox* genObjBox(Mesh*, Transform*, int*);
static BoundingBox* genWorldBox(Mesh*, int*);

static BoundingBox* genObjBox(Mesh* mesh, Transform* toWorld, int* v) {
    Transform toObj = inverseTransform(toWorld);
    Vec3 p1 = mesh -> p[v[0]];
    Vec3 p2 = mesh -> p[v[1]];
    Vec3 p3 = mesh -> p[v[2]];

    Vec3 a1 = applyTransformPoint(&toObj, &p1);
    Vec3 a2 = applyTransformPoint(&toObj, &p2);
    Vec3 a3 = applyTransformPoint(&toObj, &p3);

    Vec3* min = makeVec3(fmin(a1.x, fmin(a2.x, a3.x)), fmin(a1.y, fmin(a2.y, a3.y)), fmin(a1.z, fmin(a2.z, a3.z)));
    Vec3* max = makeVec3(fmax(a1.x, fmax(a2.x, a3.x)), fmax(a1.y, fmax(a2.y, a3.y)), fmax(a1.z, fmax(a2.z, a3.z)));

    return makeBoundingBox(min, max);
}

static BoundingBox* genWorldBox(Mesh* mesh, int* v) {
    Vec3 a1 = mesh -> p[v[0]];
    Vec3 a2 = mesh -> p[v[1]];
    Vec3 a3 = mesh -> p[v[2]];

    Vec3* min = makeVec3(fmin(a1.x, fmin(a2.x, a3.x)), fmin(a1.y, fmin(a2.y, a3.y)), fmin(a1.z, fmin(a2.z, a3.z)));
    Vec3* max = makeVec3(fmax(a1.x, fmax(a2.x, a3.x)), fmax(a1.y, fmax(a2.y, a3.y)), fmax(a1.z, fmax(a2.z, a3.z)));

    return makeBoundingBox(min, max);
}

double intersectTriangle(Ray* ray, Triangle* triangle, double tmin, double tmax, SurfaceHit* s) {
    Mesh* mesh = triangle -> mesh;
    int* arr = triangle -> vert;
    Vec3* dir = ray -> dir;
    Vec3 p0 = mesh -> p[arr[0]];
    Vec3 p1 = mesh -> p[arr[1]];
    Vec3 p2 = mesh -> p[arr[2]];
    // first, find normal vector
    Vec3 temp1, temp2;
    setVec3(&temp1, -p0.x + p1.x, -p0.y + p1.y, -p0.z + p1.z);
    setVec3(&temp2, -p0.x + p2.x, -p0.y + p2.y, -p0.z + p2.z);
    Vec3* pvec = cross(dir, &temp2); // TODO: verify orientation
    
    double det = dot(&temp1, pvec);

    // if (arr[1] == 2) {
    //     printf("=================\n");
    //     printVec3(&temp1);
    //     printf("\n");
    //     printVec3(&temp2);
    //     printf("\n");
    // }

    double invDet = 1 / det;
    Vec3* tvec = sub(ray -> from, &p0);
    double u = dot(pvec, tvec) * invDet;

    if (u < 0 || u > 1) {free(tvec);free(pvec); return INFTY;}

    Vec3* qvec = cross(tvec, &temp1);
    double v = dot(qvec, dir) * invDet;
    free(tvec);
    if (v <  0 || u + v > 1) {free(qvec);free(pvec); return INFTY;}

    double t = dot(qvec, &temp2) * invDet;
    // TODO: Fill out SurfaceHit and not free pvec (normal)
    free(qvec);
    free(pvec);
    return t > tmin && t < tmax ? t : INFTY; // make sure intersection point falls at proper point


    // Complicated way that I don't understand, might use later if it's faster
    // Vec3 p0t, p1t, p2t, d;

    // // the most accurate way to test for ray/triangle intersection is to transform the space so that the
    // // origin of the ray is (0,0,0) and the direction is the positive z axis and transform the triangle
    // // points to the same space

    // // get vertices relative to ray origin
    // setVec3(&p0t, p0.x - ray -> from -> x, p0.y - ray -> from -> y, p0.z - ray -> from -> z);
    // setVec3(&p0t, p1.x - ray -> from -> x, p1.y - ray -> from -> y, p1.z - ray -> from -> z);
    // setVec3(&p0t, p2.x - ray -> from -> x, p2.y - ray -> from -> y, p2.z - ray -> from -> z);

    // int kz = (dir -> x > dir -> y) ? (dir -> x > dir -> z) ? 0 : 2 : (dir -> y > dir -> z) ? 1 : 2;
    // int kx = (kz + 1) % 3;
    // int ky = (kx + 1) % 3;

    // setVec3(&d,   getCoordinate(dir, kx),  getCoordinate(dir, ky),  getCoordinate(dir, kz));
    // setVec3(&p0t, getCoordinate(&p0t, kx), getCoordinate(&p0t, ky), getCoordinate(&p0t, kz));
    // setVec3(&p1t, getCoordinate(&p1t, kx), getCoordinate(&p1t, ky), getCoordinate(&p1t, kz));
    // setVec3(&p2t, getCoordinate(&p2t, kx), getCoordinate(&p2t, ky), getCoordinate(&p2t, kz));

    // double sx = -d.x / d.z;
    // double sy = -d.y / d.z;
    // double sz = 1.0 / d.z;
    // p0t.x += sx * p0t.z;
    // p0t.y += sy * p0t.z;
    // p1t.x += sx * p1t.z;
    // p1t.y += sy * p1t.z;
    // p2t.x += sx * p2t.z;
    // p2t.y += sy * p2t.z;

    // double e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    // double e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    // double e2 = p0t.x * p1t.y - p0t.y * p1t.x;

    // if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0)) return 0;
    // double det = e0 + e1 + e2;
    // if (det == 0) return 0;

    // p0t.z *= sz;
    // p1t.z *= sz;
    // p2t.z *= sz;
    // double ts = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    // if (det < 0 && (ts <= 0 || ts > tmax * det)) return 0;
    // else if (det > 0 && (ts <= 0 || ts > tmax * det)) return 0;
}