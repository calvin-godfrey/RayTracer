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
Sphere* makeSphere(Vec3* center, double r, Rgb* c, Texture* t, double reflectivity, double refractivity, double refractionIndex) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    sphere -> reflectivity = reflectivity;
    sphere -> refractivity = refractivity;
    sphere -> texture = t;
    sphere -> refractionIndex = refractionIndex;
    Angles* temp = makeAngles(0, 0, 0);
    sphere -> rotation = anglesToQuaternion(temp);
    free(temp);
    return sphere;
}

Sphere* makeSphereRotation(Vec3* center, double r, Rgb* c, Texture* t, double refl, double refrac, double index, double rx, double ry, double rz) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    sphere -> reflectivity = refl;
    sphere -> refractivity = refrac;
    sphere -> texture = t;
    sphere -> refractionIndex = index;
    Angles* temp = makeAngles(rx * PI / 180, ry * PI / 180, rz * PI / 180);
    sphere -> rotation = anglesToQuaternion(temp);
    free(temp);
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
    if (sphere -> texture == NULL) {
        return makeRgb(sphere -> color -> r, sphere -> color -> g, sphere -> color -> b);
    }

    Vec3* coordinates = sub(point, sphere -> center);
    Vec3* rotated = multiply(sphere -> rotation, coordinates);

    double y = (rotated -> z / sphere -> radius + 1) / 2; // in range [0, 1]
    double angle = atan2(rotated -> y, rotated -> x); // in [-pi, pi]

    double x = (angle + PI) / (2 * PI) + 2; // transform [-pi, pi] to [0, 1], + 2 ensurse that it's positive
    x = x - (int) x;
    if (x == 1.0) x = 0.0; // edge case, texture only works on [0, 1)
    free(coordinates);
    free(rotated);
    return getPixel(sphere -> texture, x, y);
}