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

void raycast(uint16_t width, uint16_t height, FILE* file, Sphere** spheres, int sphereLength) {
    Vec3* camera = makeVec3(0.0, 0, 0);
    Vec3* light = makeVec3(0, 6, 0);

    double zMax = 1 / cos(toRads(vFOV) / 2.0);
    double yMax = 1 / cos(toRads(hFOV) / 2.0);

    for (uint16_t i = 0; i < height; i++) {
        for (uint16_t j = 0; j < width; j++) {
            int16_t zOffset = (height / 2 - i);
            double zprop = zOffset / (height / 2.0);
            double zCoord = zMax * zprop;
            int16_t yOffset = (width / 2 - j);
            double yprop = yOffset / (width / 2.0);
            double yCoord = yMax * yprop;
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
                    normalHit = normal;
                    index = i;
                }
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
            }
            free(coord);
            free(ray);
            if (normalHit != NULL) free(normalHit);
        }
    }
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