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
        return makeRgb(135, 206, 235); // sky color
    } else {
        Vec3* hitLocation = getPoint(ray, minDistance);
        Rgb* restColor = makeRgb(0, 0, 0);

        int inside = 0;
        if (dot(ray -> dir, normalHit) > 0) {
            inside = 1;
            scaleVec3(normalHit, -1);
        }

        if ((hit -> reflectivity > 0 || hit -> refractivity > 0) && depth < MAX_DEPTH) {
            Rgb* refractionColor = makeRgb(0, 0, 0);
            free(restColor);
            double ratio = -1 * dot(normalHit, ray -> dir); // both normal vectors
            double fresnel = 0;// sqrt(1 - ratio); // TODO: make this better?
            double factor = -2 * ratio;
            Vec3* reflectDir = makeVec3(ray -> dir -> x - factor * normalHit -> x, ray -> dir -> y - factor * normalHit -> y, ray -> dir -> z - factor * normalHit -> z);
            Vec3* rayDirection = add(hitLocation, reflectDir);
            Vec3* rayOrigin = makeVec3(hitLocation -> x + normalHit -> x * SMALL, hitLocation -> y + normalHit -> y * SMALL, hitLocation -> z + normalHit -> z * SMALL);
            Ray* reflectRay = makeRay(rayOrigin, rayDirection);
            Rgb* reflectionColor = trace(reflectRay, spheres, sphereLength, depth + 1);

            if (hit -> refractivity > 0) {
                free(refractionColor);
                double index = inside ? 1/1.1 : 1.1; // TODO: adjust
                double cosi = -1 * dot(normalHit, ray -> dir);
                double k = 1 - index * index * (1 - cosi * cosi);
                Vec3* refrDir = makeVec3(ray -> dir -> x * index + normalHit -> x * (index * cosi - sqrt(k)),
                                         ray -> dir -> y * index + normalHit -> y * (index * cosi - sqrt(k)),
                                         ray -> dir -> z * index + normalHit -> z * (index * cosi - sqrt(k)));
                normalize(refrDir);
                Vec3* rayOrigin = makeVec3(hitLocation -> x - normalHit -> x * SMALL, hitLocation -> y - normalHit -> y * SMALL, hitLocation -> z - normalHit -> z * SMALL);
                Vec3* rayDir = add(refrDir, hitLocation);
                Ray* refrRay = makeRay(rayOrigin, rayDir);
                refractionColor = trace(refrRay, spheres, sphereLength, depth + 1);
                freeRay(refrRay);
                free(refrRay);
                free(refrDir);
                free(rayOrigin);
                free(rayDir);
            }

            scale(reflectionColor, fresnel);
            scale(refractionColor, (1 - fresnel) * hit -> refractivity);
            multiplyColors(refractionColor, hit -> color);
            restColor = addRgb(reflectionColor, refractionColor);
            free(reflectDir);
            free(rayDirection);
            free(rayOrigin);
            freeRay(reflectRay);
            free(reflectRay);
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
        
        Rgb* baseColor = getPixelData(hit, hitLocation);
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