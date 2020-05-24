#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include "Raycast.h"
#include "Vec3.h"
#include "Ray.h"
#include "Consts.h"
#include "Color.h"

extern uint16_t width;
extern uint16_t height;
extern double aspectRatio;
extern double vFOV;
extern Vec3 camera;
extern Vec3 light;

static double toRads(double deg);
static void drawRgb(Rgb*, FILE*);
static double calcCoord(double, uint16_t, uint16_t);
static Rgb* trace(Ray*, Sphere**, int, int);

void raycast(FILE* file, Sphere** spheres, int sphereLength) {
    double zMax = 1 / cos(toRads(vFOV) / 2.0);
    double yMax = 1 / cos(toRads(hFOV) / 2.0) * (aspectRatio + 1) / 2.0; // fixes stretching

    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            double zCoord = calcCoord(zMax, height, i);
            double yCoord = calcCoord(yMax, width, j);
            Vec3* coord = makeVec3(1 + camera.x, yCoord, zCoord);
            Ray* ray = makeRay(&camera, coord);
            Rgb* color = trace(ray, spheres, sphereLength, 0);
            if (color == NULL) color = makeRgb(135, 206, 235); // sky color
            drawRgb(color, file);
            free(color);
            free(coord);
            freeRay(ray);
            free(ray);
        }
    }
}

static Rgb* trace(Ray* ray, Sphere** spheres, int sphereLength, int depth) {
    double minDistance = INFTY;
    Sphere* hit = NULL;
    Vec3* normalHit = NULL;
    int index = 0;
    // find which sphere the ray hits
    for (int i = 0; i < sphereLength; i++) {
        double before = minDistance;
        Sphere* sphere = spheres[i];
        Vec3* normal = sphereIntersect(sphere, ray, &minDistance);
        if (normal == NULL) continue;
        if (before != minDistance) {
            hit = sphere;
            free(normalHit);
            normalHit = normal;
            index = i;
        }
        if (normal != NULL && before == minDistance) free(normal);
    }
    if (normalHit == NULL) { // no hit
        return NULL;
    } else {
        Vec3* hitLocation = getPoint(ray, minDistance);
        Rgb* restColor = makeRgb(0, 0, 0);
        Rgb* baseColor = getPixelData(hit, hitLocation);

        int inside = 0;
        if (dot(ray -> dir, normalHit) > 0) {
            inside = 1;
            scaleVec3(normalHit, -1);
        }

        if ((hit -> reflectivity > 0 || hit -> refractivity > 0) && depth < MAX_DEPTH) {
            Rgb* refractionColor = makeRgb(0, 0, 0);
            free(restColor);
            double ratio = -1 * dot(normalHit, ray -> dir); // both normal vectors
            double fresnel = pow(1 - ratio, 3); // TODO: make this better?
            Vec3* reflectDir = reflectVector(normalHit, ray -> dir); // reflect ray across the normal vector
            Vec3* scaledNormal = copyScaleVec3(normalHit, SMALL);
            // We want to move the origin of the ray slightly in the direction of the normal vector
            // So that recursive calls to trace don't continually hit the same point over and over again
            Vec3* rayOrigin = add(hitLocation, scaledNormal);
            Ray* reflectRay = makeRayPointDir(rayOrigin, reflectDir);
            Rgb* reflectionColor = trace(reflectRay, spheres, sphereLength, depth + 1);
            if (reflectionColor == NULL) reflectionColor = makeRgb(255, 255, 255);

            if (hit -> refractivity > 0) {
                free(refractionColor);
                double d = 1.1;
                double index = inside ? d : 1/d; // TODO: adjust
                Vec3* refrDir = refractVector(ray -> dir, normalHit, index);
                if (refrDir == NULL) {
                    refractionColor = makeRgb(0, 0, 0);
                } else {
                    normalize(refrDir);
                    Vec3* origin = makeVec3(hitLocation -> x - normalHit -> x * SMALL, hitLocation -> y - normalHit -> y * SMALL, hitLocation -> z - normalHit -> z * SMALL);
                    Ray* refrRay = makeRayPointDir(origin, refrDir);
                    refractionColor = trace(refrRay, spheres, sphereLength, depth + 1);
                    if (refractionColor == NULL) refractionColor = makeRgb(255, 255, 255);
                    freeRay(refrRay);
                    free(refrRay);
                    free(refrDir);
                    free(origin);
                }
            }

            scale(reflectionColor, fresnel);
            scale(refractionColor, (1 - fresnel) * hit -> refractivity);
            
            multiplyColors(refractionColor, baseColor);
            restColor = addRgb(reflectionColor, refractionColor);
            free(reflectDir);
            free(rayOrigin);
            freeRay(reflectRay);
            free(reflectRay);
            free(scaledNormal);
            free(reflectionColor);
            free(refractionColor);
        }

        Ray* lightRay = makeRay(&light, hitLocation);
        double dummy = INFTY;
        int shadow = 0;
        // Check all other spheres to see if there is a shadow
        for (int i = 0; i < sphereLength; i++) {
            if (i == index) continue;
            Vec3* normal = sphereIntersect(spheres[i], lightRay, &dummy);
            if (normal != NULL && onLine(lightRay, dummy) == 1) {
                shadow = 1;
                dummy = INFTY;
            }
            if (normal != NULL) free(normal);
        }
        Vec3* lightDirection = sub(&light, hitLocation);
        normalize(lightDirection);
        double d = dot(normalHit, lightDirection);
        
        scale(baseColor, shadow == 1 ? 0.0 : fmax(0.0, d));
        Rgb* finalColor = addRgb(baseColor, restColor);
        free(restColor);
        free(baseColor);
        free(hitLocation);
        free(lightDirection);
        freeRay(lightRay);
        free(lightRay);
        free(normalHit);
        return finalColor;
    }
}

double toRads(double deg) {
    return deg * PI / 180;
}

static void drawRgb(Rgb* rgb, FILE* file) {
    fputc(rgb -> b, file);
    fputc(rgb -> g, file);
    fputc(rgb -> r, file);
}

static double calcCoord(double max, uint16_t coord, uint16_t value) {
    int16_t offset = (coord / 2 - value);
    double prop = offset / (coord / 2.0);
    return max * prop;
}