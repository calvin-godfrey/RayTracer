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
