#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Consts.h"
#include "TriangleMesh.h"
#include "../math/Vec3.h"
#include "../util/Parser.h"
#include "../Color.h"

SurfaceHit* meshIntersect(Mesh* mesh, Ray* ray, double* dist) {
    SurfaceHit* ans = NULL;
    if (mesh -> box != NULL && !intersectsBox(ray, mesh -> box)) return NULL;
    double smallest = *dist;
    for (int i = 0; i < mesh -> nTriangles; i++) {
        int* vertices = 3 * i + mesh -> pInd;
        int* norm = mesh -> nInd == NULL ? NULL : 3 * i + mesh -> nInd;
        int* uvArr = mesh -> uvInd == NULL ? NULL : 3 * i + mesh -> uvInd;
        SurfaceHit* hit = makeEmptySurfaceHit();
        double curr = intersectsTriangle(mesh, ray, vertices, norm, uvArr, 0, INFTY, hit);
        if (curr != INFTY && curr < smallest) {
            smallest = curr;
            if (ans != NULL) free(ans);
            ans = hit;
            if (mesh -> uvInd[0] == -1) {
                ans -> color = makeRgb(255, 0, 0); // texture not defined
            } else {
                Material* m = mesh -> mats[mesh -> matInd[i]];
                ans -> color = getPixel(m -> texture, hit -> uv -> x, hit -> uv -> y);
            }
        } else {
            free(hit);
        }
    }
    *dist = smallest;
    return ans;
}

double intersectsTriangle(Mesh* mesh, Ray* ray, int* vert, int* norm, int* uvArr, double tmin, double tmax, SurfaceHit* s) {
    Vec3* dir = ray -> dir;
    Vec3 p0 = mesh -> p[vert[0]];
    Vec3 p1 = mesh -> p[vert[1]];
    Vec3 p2 = mesh -> p[vert[2]];
    // first, find normal vector
    Vec3 temp1, temp2;
    setVec3(&temp1, -p0.x + p1.x, -p0.y + p1.y, -p0.z + p1.z);
    setVec3(&temp2, -p0.x + p2.x, -p0.y + p2.y, -p0.z + p2.z);
    Vec3* pvec = cross(dir, &temp2); // TODO: verify orientation
    
    double det = dot(&temp1, pvec);

    double invDet = 1 / det;
    Vec3* tvec = sub(ray -> from, &p0);
    double u = dot(pvec, tvec) * invDet;

    if (u < 0 || u > 1) {free(tvec);free(pvec); return INFTY;}

    Vec3* qvec = cross(tvec, &temp1);
    double v = dot(qvec, dir) * invDet;
    free(tvec);
    if (v <  0 || u + v > 1) {free(qvec);free(pvec); return INFTY;}

    double t = dot(qvec, &temp2) * invDet;

    free(qvec);
    free(pvec);
    if (!(t > tmin && t < tmax)) return INFTY; // doesn't fall in proper boundary
    s -> uv = makeVec3(0, 0, 0);
    s -> time = t;
    if (1){//norm[0] == -1) { // normal not defined
        s -> n = cross(&temp1, &temp2); // normal is the one we calculated
    } else { // otherwise interpolate
        printf("no\n");
        Vec3 n1 = mesh -> n[norm[0]];
        Vec3 n2 = mesh -> n[norm[1]];
        Vec3 n3 = mesh -> n[norm[2]];
        scaleVec3(&n1, 1 - u - v);
        scaleVec3(&n1, u);
        scaleVec3(&n1, v);
        Vec3* n = add3(&n1,  &n2, &n3);
        normalize(n);
        s -> n = n;
    }
    s -> p = getPoint(ray, t);
    if (uvArr[0] != -1) { // calculate proper u/v coordinates of hit
        Vec3 uv1 = mesh -> uv[uvArr[0]];
        Vec3 uv2 = mesh -> uv[uvArr[1]];
        Vec3 uv3 = mesh -> uv[uvArr[2]];
        scaleVec3(&uv1, 1 - u - v);
        scaleVec3(&uv2, u);
        scaleVec3(&uv3, v);
        Vec3* ans = add3(&uv1, &uv2, &uv3);
        s -> uv = ans;
    } else {
        s -> uv = makeVec3(-1, -1, -1);
    }
    return t;
}