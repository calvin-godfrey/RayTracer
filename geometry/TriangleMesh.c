#include <stdio.h>
#include <stdlib.h>
#include "../Consts.h"
#include "TriangleMesh.h"
#include "../math/Vec3.h"
#include "../util/Parser.h"

Vec3 meshIntersect(Mesh* mesh, Ray* ray, double* dist) {
    Vec3 ans = {INFTY, INFTY, INFTY, 0};
    if (mesh -> box != NULL && !intersectsBox(ray, mesh -> box)) return ans;
    double smallest = *dist;
    for (int i = 0; i < mesh -> nTriangles; i++) {
        Triangle triangle;
        triangle.mesh = mesh;
        triangle.vert = 3 * i + mesh -> vertInd;
        double curr = intersectTriangle(ray, &triangle, 0, INFTY, NULL);
        // if (curr < INFTY) printf("dist: %f, %d\n", curr, triangle.vert[1]);
        if (curr != INFTY && curr < smallest) {
            smallest = curr;
            // calculate normal using points
            // printf("%d\n", triangle.vert[1]);
            Vec3 a = mesh -> p[triangle.vert[0]];
            Vec3 b = mesh -> p[triangle.vert[1]];
            Vec3 c = mesh -> p[triangle.vert[2]];
            Vec3* ab = sub(&a, &b);
            Vec3* ac = sub(&a, &c);
            Vec3* cr = cross(ab, ac);
            copyVec3(&ans, cr);
            free(ab);
            free(ac);
            free(cr);
        }
    }
    *dist = smallest;
    return ans;
}