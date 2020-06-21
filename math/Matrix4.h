#ifndef MATRIX_H
#define MATRIX_H
#include "Vec3.h"

struct _Matrix4 {
    double mat[4][4];
};

typedef struct _Matrix4 Matrix4;

Matrix4 makeMatrix4(double m11, double m12, double m13, double m14,
                     double m21, double m22, double m23, double m24,
                     double m31, double m32, double m33, double m34,
                     double m41, double m42, double m43, double m44);

Matrix4 makeMatrix4Single(double);

Matrix4 tranpose(Matrix4*);
Matrix4 multiplyMatrix(Matrix4*, Matrix4*);
Matrix4 inverse(Matrix4*);
void copyMatrix(Matrix4*, Matrix4*);
double det(Matrix4*);
void displayMatrix(Matrix4*);
Vec3 multiplyPoint(Matrix4*, Vec3*);
Vec3 multiplyVector(Matrix4*, Vec3*);
Vec3 multiplyNormal(Matrix4*, Vec3*);

#endif