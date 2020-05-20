#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Sphere.h"
#include "Consts.h"

/**
 * Returns a pointer to a Sphere struct object. Note that in the logic used, it is assumed that
 * if a sphere is refractive (i.e., has some transparency), it is also reflective. Also note
 * that a sphere cannot have both a texture AND color. Texture has higher priority; if it is
 * null, only then is its color used.
 */
Sphere* makeSphere(Vec3* center, double r, Rgb* c, Texture* t, double reflectivity, double refractivity) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    sphere -> reflectivity = reflectivity;
    sphere -> refractivity = refractivity;
    sphere -> texture = t;
    return sphere;
}

void freeSphere(Sphere* sphere) {
    free(sphere -> center);
    freeTexture(sphere -> texture);
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
        normalize(normal);
        free(L);
        free(point);
        return normal;
    }    
}

Rgb* getPixelData(Sphere* sphere, Vec3* point) {
    // TODO: Rotation
    if (sphere -> texture == NULL) return sphere -> color;
    Vec3* coordinates = sub(point, sphere -> center);
    double y = (coordinates -> z / sphere -> radius + 1.0) / 2;
    double angle = atan2(coordinates -> y, coordinates -> x); // in [-pi, pi]
    // TODO: Decide where I want 0 to be on the texture
    double x = (angle + PI) / (2 * PI); // transform [-pi, pi] to [0, 1]
    x += 0.3;
    x = x - (int) x;
    if (x == 1.0) x = 0.0; // edge case, texture only works on [0, 1)
    free(coordinates);
    return getPixel(sphere -> texture, x, y);
}