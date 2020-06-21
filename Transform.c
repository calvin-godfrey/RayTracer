#include <stdlib.h>
#include "Transform.h"
#include "../Consts.h"

Transform makeTransform(Matrix4* mat) {
    Transform t;
    t.mat = mat;
    Matrix4* inv = malloc(sizeof(Matrix4));
    Matrix4 temp = inverse(mat);
    copyMatrix(inv, &temp);
    t.inv = inv;
    return t;
}

Transform makeTransformInverse(Matrix4* mat, Matrix4* inv) {
    Transform t;
    t.mat = mat;
    t.inv = inv;
    return t;
}

Transform inverseTransform(Transform* t) {
    return makeTransformInverse(t -> inv, t -> mat);
}

Vec3 applyTransformPoint(Transform* t, Vec3* v) {
    return multiplyPoint(t -> mat, v);
}

Vec3 applyTransformVec(Transform* t, Vec3* v) {
    return multiplyVector(t -> mat, v);
}

Vec3 applyTransformNormal(Transform* t, Vec3* v) {
    // must apply transpose of inverse to normal
    return multiplyNormal(t -> inv, v);
}

Ray applyTransformRay(Transform* t, Ray* ray) {
    Vec3 o = applyTransformPoint(t, ray -> from);
    Vec3 dir = applyTransformVec(t, ray -> dir);
    Ray ans;
    ans.from = makeVec3(0, 0, 0);
    ans.dir  = makeVec3(0, 0, 0);
    copyVec3(ans.from, &o);
    copyVec3(ans.dir, &dir);
    return ans;
}

BoundingBox applyTransformBoundingBox(Transform* t, BoundingBox* box) {
    // must make sure that the resulting boundingbox maintains the bounding property. To do this,
    // transform each of the eight corners, and union them together
    Vec3 before;
    Vec3 after;
    BoundingBox ans;
    ans.min = makeVec3(0, 0, 0);
    ans.max = makeVec3(0, 0, 0);
    setVec3(ans.min, INFTY, INFTY, INFTY);
    setVec3(ans.max, -INFTY, -INFTY, -INFTY); // ensures that Union will work
    // Do the following eight times, one for each corner
    setVec3(&before, box -> min -> x, box -> min -> y, box -> min -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> max -> x, box -> min -> y, box -> min -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> min -> x, box -> max -> y, box -> min -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> max -> x, box -> max -> y, box -> min -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> min -> x, box -> min -> y, box -> max -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> max -> x, box -> min -> y, box -> max -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> min -> x, box -> max -> y, box -> max -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);

    setVec3(&before, box -> max -> x, box -> max -> y, box -> max -> z);
    after = applyTransformPoint(t, &before);
    ans = Union(&ans, &after);
    return ans;
}