#include <stdio.h>
#include <stdlib.h>
#include "../Consts.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "../math/Vec3.h"

Mesh* makeMesh(char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", path);
        return NULL;
    }
    // we first have to count everything before we can malloc memory
    char* line = calloc(LINE_LENGTH, sizeof(char));
    int numVert = 0;
    while (1) {
        if (fgets(line, LINE_LENGTH, file) == NULL) break;
        
    }
}

Vec3 meshIntersect(Mesh* mesh, Ray* ray, double* dist) {
    Vec3 bad = {INFTY, INFTY, INFTY, 0};
    if (!intersectsBox(ray, mesh -> box)) return bad;
    for (int i = 0; i < mesh -> nTriangles; i++) {
        Triangle triangle;
        int* arr = malloc(sizeof(int) * 3);
        arr[0] = mesh -> vertInd[3 * i];
        arr[1] = mesh -> vertInd[3 * i + 1];
        arr[2] = mesh -> vertInd[3 * i + 2];
        triangle.vert = arr;
        triangle.mesh = mesh;
        if (intersectTriangle(ray, &triangle, 0, INFTY, NULL)) {
            *dist = 0; // force intersection
            // calculate normal using points
            Vec3 a = mesh -> p[arr[0]];
            Vec3 b = mesh -> p[arr[1]];
            Vec3 c = mesh -> p[arr[2]];
            Vec3* ab = sub(&a, &b);
            Vec3* ac = sub(&a, &c);
            Vec3* cr = cross(ab, ac);
            Vec3 ans;
            copyVec3(&ans, cr);
            free(ab);
            free(ac);
            free(cr);
            return ans;
        }
    }
    return bad;
}