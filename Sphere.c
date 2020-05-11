#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Sphere.h"

Sphere* makeSphere(Vec3* center, double r, Rgb* c) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    return sphere;
}

void freeSphere(Sphere* sphere) {
    free(sphere -> center);
    free(sphere -> color);
}

Vec3* sphereIntersect(Sphere* sphere, Ray* ray, double* minDistance) {
    Vec3* L = sub(sphere -> center, ray -> from);
    double tca = dot(L, ray -> dir);
    double d2 = dot(L, L) - tca * tca;
    if (d2 > sphere -> radius * sphere -> radius) { // no intersect
        free(L);
        return NULL;
    } else {
        double thc = sqrt(sphere -> radius * sphere -> radius - d2);
        double x0 = tca - thc;
        double x1 = tca + thc;
        double ans;
        if (x0 < x1) {
            if (x0 < 0) {
                if (x1 < 0) {
                    free(L);
                    return NULL;
                }
                ans = x1;
            } else {
                ans = x0;
            }
        } else {
            if (x1 < 0) {
                if (x0 < 0) {
                    free(L);
                    return NULL;
                }
                ans = x0;
            } else {
                ans = x1;
            }
        }
        if (ans < *minDistance) *minDistance = ans;
        Vec3* point = getPoint(ray, ans);
        Vec3* normal = sub(point, sphere -> center);
        // if (sphere -> color -> r != 0 && ray -> from -> y == 100.0) {
        //     if (normal == NULL) printf("BADBADBAD\n");
        //     printf("hdoifjsoere\n");
        // }
        normalize(normal);
        free(L);
        free(point);
        return normal;
    }    
}