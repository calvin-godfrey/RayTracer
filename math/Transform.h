#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Matrix4.h"
#include "Vec3.h"
#include "../geometry/Ray.h"
#include "../geometry/BoundingBox.h"

struct _Transform {
    Matrix4* mat;
    Matrix4* inv;
};

typedef struct _Transform Transform;

Transform makeTransform(Matrix4*);
Transform makeTransformInverse(Matrix4*, Matrix4*);
Transform inverseTransform(Transform*);
Vec3 applyTransformPoint(Transform*, Vec3*);
Vec3 applyTransformVec(Transform*, Vec3*);
Vec3 applyTransformNormal(Transform*, Vec3*);
Ray applyTransformRay(Transform*, Ray*);
BoundingBox applyTransformBox(Transform*, BoundingBox*);

#endif