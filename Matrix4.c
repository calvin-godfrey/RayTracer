#include <stdlib.h>
#include <stdio.h>
#include "Matrix4.h"

static Matrix4 invGeneral(Matrix4*);

Matrix4 makeMatrix4(double m11, double m12, double m13, double m14,
                     double m21, double m22, double m23, double m24,
                     double m31, double m32, double m33, double m34,
                     double m41, double m42, double m43, double m44) {

    Matrix4 mat = {
        {{m11, m12, m13, m14},
         {m21, m22, m23, m24},
         {m31, m32, m33, m34},
         {m41, m42, m43, m44}}
    };
    return mat;
}

Matrix4 makeMatrix4Single(double d) {
    return makeMatrix4(d, d, d, d, d, d, d, d, d, d, d, d, d, d, d, d);
}

Matrix4 transpose(Matrix4* mat) {
    return makeMatrix4(mat -> mat[0][0], mat -> mat[1][0], mat -> mat[2][0], mat -> mat[3][0],
                       mat -> mat[0][1], mat -> mat[1][1], mat -> mat[2][1], mat -> mat[3][1],
                       mat -> mat[0][2], mat -> mat[1][2], mat -> mat[2][2], mat -> mat[3][2],
                       mat -> mat[0][3], mat -> mat[1][3], mat -> mat[2][3], mat -> mat[3][3]);
}

Matrix4 multiplyMatrix(Matrix4* a, Matrix4* b) {
    Matrix4 ans = makeMatrix4Single(0);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            ans.mat[i][j] = a -> mat[i][0] * b -> mat[0][j] + 
                            a -> mat[i][1] * b -> mat[1][j] + 
                            a -> mat[i][2] * b -> mat[2][j] + 
                            a -> mat[i][3] * b -> mat[3][j];
        }
    }

    return ans;
}

double det(Matrix4* mat) {
    double xx = mat -> mat[0][0];
    double xy = mat -> mat[0][1];
    double xz = mat -> mat[0][2];
    double xw = mat -> mat[0][3];

    double yx = mat -> mat[1][0];
    double yy = mat -> mat[1][1];
    double yz = mat -> mat[1][2];
    double yw = mat -> mat[1][3];

    double zx = mat -> mat[2][0];
    double zy = mat -> mat[2][1];
    double zz = mat -> mat[2][2];
    double zw = mat -> mat[2][3];

    double wx = mat -> mat[3][0];
    double wy = mat -> mat[3][1];
    double wz = mat -> mat[3][2];
    double ww = mat -> mat[3][3];

    return xw * yz * zy * wx - xz * yw * zy * wx - xw * yy * zz * wx + xy * yw * zz * wx +
           xz * yy * zw * wx - xy * yz * zw * wx - xw * yz * zx * wy + xz * yw * zx * wy +
           xw * yx * zz * wy - xx * yw * zz * wy - xz * yx * zw * wy + xx * yz * zw * wy +
           xw * yy * zx * wz - xy * yw * zx * wz - xw * yx * zy * wz + xx * yw * zy * wz +
           xy * yx * zw * wz - xx * yy * zw * wz - xz * yy * zx * ww + xy * yz * zx * ww +
           xz * yx * zy * ww - xx * yz * zy * ww - xy * yx * zz * ww + xx * yy * zz * ww;
}

static Matrix4 invGeneral(Matrix4* mat) {
    double xx = mat -> mat[0][0];
    double xy = mat -> mat[0][1];
    double xz = mat -> mat[0][2];
    double xw = mat -> mat[0][3];

    double yx = mat -> mat[1][0];
    double yy = mat -> mat[1][1];
    double yz = mat -> mat[1][2];
    double yw = mat -> mat[1][3];

    double zx = mat -> mat[2][0];
    double zy = mat -> mat[2][1];
    double zz = mat -> mat[2][2];
    double zw = mat -> mat[2][3];

    double wx = mat -> mat[3][0];
    double wy = mat -> mat[3][1];
    double wz = mat -> mat[3][2];
    double ww = mat -> mat[3][3];

    double det;
    double d12, d13, d23, d24, d34, d41;

    Matrix4 temp;

    d12 = (xz * yw - xw * yz);
    d13 = (xz * zw - xw * zz);
    d23 = (yz * zw - yw * zz);
    d24 = (yz * ww - yw * wz);
    d34 = (zz * ww - zw * wz);
    d41 = (wz * wz - ww * xz);

    temp.mat[0][0] =  (yy * d34 - zy * d24 + wy * d23);
    temp.mat[0][1] = -(xy * d34 + zy * d41 + wy * d13);
    temp.mat[0][2] =  (xy * d24 + yy * d41 + wy * d12);
    temp.mat[0][3] = -(xy * d23 - yy * d13 + zy * d12);

    det = xx * temp.mat[0][0] + yx * temp.mat[0][1] + zx * temp.mat[0][2] + wx * temp.mat[0][3];

    if (det == 0.0) return makeMatrix4Single(0.0);

    double invDet = 1 / det;
    temp.mat[0][0] *= invDet;
    temp.mat[0][1] *= invDet;
    temp.mat[0][2] *= invDet;
    temp.mat[0][3] *= invDet;

    temp.mat[1][0] = -(yx * d34 - zx * d24 + wx * d23) * invDet;
    temp.mat[1][1] =  (xx * d34 + zx * d41 + wx * d13) * invDet;
    temp.mat[1][2] = -(xx * d24 + yx * d41 + wx * d12) * invDet;
    temp.mat[1][3] =  (xx * d23 - yx * d13 + zx * d12) * invDet;

    d12 = xx * yy - xy * yx;
    d13 = xx * zy - xy * zx;
    d23 = yx * zy - yy * zx;
    d24 = yx * wy - yy * wx;
    d34 = zx * wy - zy * wx;
    d41 = wx * xy - wy * xx;

    temp.mat[2][0] =  (yw * d34 - zw * d24 + ww * d23) * invDet;
    temp.mat[2][1] = -(xw * d34 + zw * d41 + ww * d13) * invDet;
    temp.mat[2][2] =  (xw * d24 + yw * d41 + ww * d12) * invDet;
    temp.mat[2][3] = -(xw * d23 - yw * d13 + zw * d12) * invDet;

    temp.mat[3][0] = -(yz * d34 - zz * d24 + wz * d23) * invDet;
    temp.mat[3][1] =  (xz * d34 + zz * d41 + wz * d13) * invDet;
    temp.mat[3][2] = -(xz * d24 + yz * d41 + wz * d12) * invDet;
    temp.mat[3][3] =  (xz * d23 - yz * d13 + zz * d12) * invDet;

    return temp;
}

Matrix4 inverse(Matrix4* mat) {
    double xx = mat -> mat[0][0];
    double xy = mat -> mat[0][1];
    double xz = mat -> mat[0][2];
    double xw = mat -> mat[0][3];

    double yx = mat -> mat[1][0];
    double yy = mat -> mat[1][1];
    double yz = mat -> mat[1][2];
    double yw = mat -> mat[1][3];

    double zx = mat -> mat[2][0];
    double zy = mat -> mat[2][1];
    double zz = mat -> mat[2][2];
    double zw = mat -> mat[2][3];

    double wx = mat -> mat[3][0];
    double wy = mat -> mat[3][1];
    double wz = mat -> mat[3][2];
    double ww = mat -> mat[3][3];

    if (xw != 0.0 || yw != 0.0 || zw != 0.0 || ww != 1.0) return invGeneral(mat);

    Matrix4 temp;

    temp.mat[0][0] = zz * ww - wz * zw;
    temp.mat[0][1] = wz * yw - yz * ww;
    temp.mat[0][2] = yz * zw - zz * yw;

    double det = yy * temp.mat[0][0] + zy * temp.mat[0][1] + wy * temp.mat[0][2];

    if (det == 0.0) return makeMatrix4Single(0.0);

    det = 1 / det;

    temp.mat[0][0] *= det;
    temp.mat[0][1] *= det;
    temp.mat[0][2] *= det;
    temp.mat[0][3] = 0.0;

    double im11 = xx * det;
    double im12 = yx * det;
    double im13 = zx * det;
    double im14 = wx * det;

    temp.mat[1][0] = im13 * yz - im12 * zz;
    temp.mat[1][1] = im11 * zz - im13 * xz;
    temp.mat[1][2] = im12 * xz - im11 * yz;
    temp.mat[1][3] = 0.0f;

    double d12 = im11 * yy - xy * im12;
    double d13 = im11*zy - xy*im13;
	double d23 = im12*zy - yy*im13;
	double d24 = im12*wy - yy*im14;
	double d34 = im13*wy - zy*im14;
	double d41 = im14*xy - wy*im11;

	temp.mat[2][0] =  d23;
	temp.mat[2][1] = -d13;
	temp.mat[2][2] = d12;
	temp.mat[2][3] = 0.0f;

	temp.mat[3][0] = -(yz * d34 - zz * d24 + wz * d23);
	temp.mat[3][1] =  (xz * d34 + zz * d41 + wz * d13);
	temp.mat[3][2] = -(xz * d24 + yz * d41 + wz * d12);
	temp.mat[3][3] =  1.0f;

    return temp;
}

/**
 * Copy matrix from second paramter into first parameter
 */
void copyMatrix(Matrix4* to, Matrix4* from) {
    to -> mat[0][0] = from -> mat[0][0];
    to -> mat[0][1] = from -> mat[0][1];
    to -> mat[0][2] = from -> mat[0][2];
    to -> mat[0][3] = from -> mat[0][3];

    to -> mat[1][0] = from -> mat[1][0];
    to -> mat[1][1] = from -> mat[1][1];
    to -> mat[1][2] = from -> mat[1][2];
    to -> mat[1][3] = from -> mat[1][3];

    to -> mat[2][0] = from -> mat[2][0];
    to -> mat[2][1] = from -> mat[2][1];
    to -> mat[2][2] = from -> mat[2][2];
    to -> mat[2][3] = from -> mat[2][3];

    to -> mat[3][0] = from -> mat[3][0];
    to -> mat[3][1] = from -> mat[3][1];
    to -> mat[3][2] = from -> mat[3][2];
    to -> mat[3][3] = from -> mat[3][3];
}

void displayMatrix(Matrix4* mat) {
    printf("%8.5f %8.5f %8.5f %8.5f\n", mat -> mat[0][0], mat -> mat[0][1], mat -> mat[0][2], mat -> mat[0][3]);
    printf("%8.5f %8.5f %8.5f %8.5f\n", mat -> mat[1][0], mat -> mat[1][1], mat -> mat[1][2], mat -> mat[1][3]);
    printf("%8.5f %8.5f %8.5f %8.5f\n", mat -> mat[2][0], mat -> mat[2][1], mat -> mat[2][2], mat -> mat[2][3]);
    printf("%8.5f %8.5f %8.5f %8.5f\n", mat -> mat[3][0], mat -> mat[3][1], mat -> mat[3][2], mat -> mat[3][3]);
}

Vec3 multiplyPoint(Matrix4* mat, Vec3* p) {
    double x = p -> x, y = p -> y, z = p -> z;
    double xp = mat -> mat[0][0] * x + mat -> mat[0][1] * y + mat -> mat[0][2] * z + mat -> mat[0][3];
    double yp = mat -> mat[1][0] * x + mat -> mat[1][1] * y + mat -> mat[1][2] * z + mat -> mat[1][3];
    double zp = mat -> mat[2][0] * x + mat -> mat[2][1] * y + mat -> mat[2][2] * z + mat -> mat[2][3];
    double wp = mat -> mat[3][0] * x + mat -> mat[3][1] * y + mat -> mat[3][2] * z + mat -> mat[3][3];

    double invWp = 1/wp;
    Vec3 ans;
    if (wp == 1) setVec3(&ans, xp, yp, zp);
    else         setVec3(&ans, xp * invWp, yp * invWp, zp * invWp);
    return ans;
}

Vec3 multiplyVector(Matrix4* mat, Vec3* v) {
    double x = v -> x, y = v -> y, z = v -> z;
    Vec3 ans;
    setVec3(&ans, mat -> mat[0][0] * x + mat -> mat[0][1] * y + mat -> mat[0][2] * z,
                  mat -> mat[1][0] * x + mat -> mat[1][1] * y + mat -> mat[1][2] * z,
                  mat -> mat[2][0] * x + mat -> mat[2][1] * y + mat -> mat[2][2] * z
    );
    return ans;
}

Vec3 multiplyNormal(Matrix4* mat, Vec3* n) {
    double x = n -> x, y = n -> y, z = n -> z;
    Vec3 ans;
    setVec3(&ans, mat -> mat[0][0] * x + mat -> mat[1][0] * y + mat -> mat[2][0] * z,
                  mat -> mat[0][1] * x + mat -> mat[1][1] * y + mat -> mat[2][1] * z,
                  mat -> mat[0][2] * x + mat -> mat[1][2] * y + mat -> mat[2][2] * z
    );
    return ans;
}