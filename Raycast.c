#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include "Raycast.h"
#include "Vec3.h"
#include "Ray.h"
#include "Consts.h"
#include "Color.h"

static double toRads(double deg);
static void putShade(char, FILE*);
static void drawRgb(Rgb*, FILE*);
static double calcCoord(double, uint16_t, uint16_t);

void raycast(uint16_t width, uint16_t height, FILE* file, Sphere** spheres, int sphereLength) {
    Vec3* camera = makeVec3(-10, 0, 0);
    Vec3* light = makeVec3(0, 60, 0);

    double zMax = 1 / cos(toRads(vFOV) / 2.0);
    double yMax = 1 / cos(toRads(hFOV) / 2.0);

    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            double zCoord = calcCoord(zMax, height, i);
            double yCoord = calcCoord(yMax, width, j);
            Vec3* coord = makeVec3(1 + camera -> x, yCoord, -zCoord);
            double minDistance = 10000.0;
            Ray* ray = makeRay(camera, coord);
            Sphere* hit = NULL;
            Vec3* normalHit = NULL;
            int index = 0;
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
            if (normalHit == NULL) {
                putShade(255, file);
            } else {
                Vec3* hitLocation = getPoint(ray, minDistance);
                Ray* lightRay = makeRay(light, hitLocation);
                double dummy = 10000.0;
                int shadow = 0;
                for (int i = 0; i < sphereLength; i++) {
                    if (i == index) continue;
                    Vec3* normal = sphereIntersect(spheres[i], lightRay, &dummy);
                    if (normal != NULL && onLine(lightRay, dummy) == 1) {
                        shadow = 1;
                        dummy = 10000.0;
                    }
                    if (normal != NULL) free(normal);
                }
                Vec3* lightDirection = sub(light, hitLocation);
                normalize(lightDirection);
                double d = dot(normalHit, lightDirection);
                
                Rgb* finalColor = makeRgb(hit -> color -> r, hit -> color -> g, hit -> color -> b);
                scale(finalColor, shadow == 1 ? 0.0 : fmax(0.0, d));
                drawRgb(finalColor, file);
                free(finalColor);
                free(hitLocation);
                free(lightDirection);
                freeRay(lightRay);
                free(lightRay);
            }
            free(coord);
            freeRay(ray);
            free(ray);
            if (normalHit != NULL) free(normalHit);
        }
    }
    free(camera);
    free(light);
}

double toRads(double deg) {
    return deg * PI / 180;
}

static void putShade(char c, FILE* file) {
    fputc(c, file);
    fputc(c, file);
    fputc(c, file);
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