#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>
#include "Raycast.h"
#include "Ray.h"
#include "Consts.h"

extern uint16_t width;
extern uint16_t height;
extern double aspectRatio;
extern double vFOV;
extern Vec3 cameraLocation;
extern Vec3 cameraDirection;
extern double cameraOrientation;
extern Vec3 light;

static double toRads(double deg);
static int16_t calcCoord(uint16_t, uint16_t);
static Rgb* trace(Ray*, Sphere**, int, int);
static ThreadArgs* makeThreadArgs(Vec3*, Vec3*, Sphere**, int, unsigned char*, uint16_t);
static void* threadCall(void*);

void raycast(FILE* file, Sphere** spheres, int sphereLength) {
    // The camera direction uniquely defines a plane that will represent the
    // viewpoint of the renderer.

    // In order to find a point on the above plane to draw a ray through, we
    // need two vectors in order to make an orthonormal basis of the plane
    Vec3** temp = getOrthogonalVectors(&cameraDirection);
    Vec3* v1 = temp[0];
    Vec3* v2 = temp[1];

    // We then rotate these two basis vectors around the cameraDirection to
    // account for the orientation of the camera
    Vec3* r1 = rotate(v1, &cameraDirection, cameraOrientation);
    Vec3* r2 = rotate(v2, &cameraDirection, cameraOrientation);

    // Assuming that the plane resides a distance of one from cameraLocation, these
    // two variables represent how far in each of the two perpendicular directions
    // (v1 and v2) you can go. In other words, they describe the deviation from the
    // "middle" of the plane in the four corners.
    double zMax = 1 / cos(toRads(vFOV) / 2.0);
    double yMax = 1 / cos(toRads(hFOV) / 2.0) * (aspectRatio + 1) / 2.0; // fixes stretching

    // This compresses the two unit vectors so that moving one pixel in each direction
    // means adding another copy of r1 or r2 to find the endpoint.
    scaleVec3(r1, zMax * 2 / height);
    scaleVec3(r2, yMax * 2 / width);

    // This is where the pixel data will eventually be stored. It's faster to store
    // the raw bytes then do a single fwrite call at the end
    unsigned char* pixels = calloc(height * width * 3, sizeof(unsigned char));

    // We want to make a thread for each row of the picture to speed up processing time.
    // The fullArgs array is because pthread only allows a single void* parameter, so
    // convention is to make a struct containing the actual parameters
    pthread_t* ids = calloc(height, sizeof(pthread_t));
    ThreadArgs** fullArgs = calloc(height, sizeof(ThreadArgs*));

    for (uint16_t i = 0; i < height; i++) {
        // The arguments are the two basis vectors, sphere data, pixel array, and row number
        ThreadArgs* args = makeThreadArgs(r1, r2, spheres, sphereLength, pixels, i);
        fullArgs[i] = args;

        // create the thread
        if (pthread_create(&ids[i], NULL, threadCall, (void*) args) != 0) {
            printf("Something went wrong: %"PRIu16"\n", i);
            return;
        }
    }

    // make sure that all of the threads finish execution before writing pixel data to file
    for (uint16_t i = 0; i < height; i++) {
        pthread_join(ids[i], NULL);
    }

    fwrite(pixels, sizeof(unsigned char), 3 * width * height, file);
    free(pixels);
    free(ids);

    for (int i = 0; i < height; i++) free(fullArgs[i]);
    free(fullArgs);
}

static void* threadCall(void* args) {
    ThreadArgs* arguments = (ThreadArgs*) args;
    uint16_t i = arguments -> row;

    // we want to do as much computation as possible outside the loop
    double scale1 = calcCoord(height, i);
    Vec3* scaledV1 = copyScaleVec3(arguments -> step1, scale1);
    // Because we transformed step1 and step2 into "pixel-space", so to speak,
    // for V2 (which increases with j), we can simply start it at its leftmost
    // value and increment it with j
    Vec3* scaledV2 = copyScaleVec3(arguments -> step2, calcCoord(width, 0));
    Vec3* step = copyScaleVec3(arguments -> step2, -1);
    Vec3* temp = add(scaledV1, &cameraDirection);
    int index = 3 * i * width;
    Vec3* dir = makeVec3(0, 0, 0);
    Ray* ray = malloc(sizeof(Ray));
    ray -> from = &cameraLocation;
    ray -> pointDir = 0;
    
    for (uint16_t j = 0; j < width; j++) {
        incVec3(scaledV2, step);
        setAddVec3(dir, temp, scaledV2);
        normalize(dir);
        Vec3* to = add(&cameraLocation, dir);
        ray -> to = to;
        ray -> dir = dir;
        Rgb* color = trace(ray, arguments -> spheres, arguments -> sphereLength, 0);
        if (color == NULL) color = makeRgb(135, 206, 235); // sky color
        arguments -> pixels[index++] = color -> b;
        arguments -> pixels[index++] = color -> g;
        arguments -> pixels[index++] = color -> r;
        free(color);
        free(to);
    }

    freeRay(ray);
    free(ray);
    free(scaledV1);
    free(scaledV2);
    free(temp);
    free(step);
    return NULL;
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

        double dotProd = dot(ray -> dir, normalHit);

        if (dotProd > 0) {
            inside = 1;
            scaleVec3(normalHit, -1);
            dotProd *= -1;
        }

        if ((hit -> reflectivity > 0 || hit -> refractivity > 0) && depth < MAX_DEPTH) {
            Rgb* refractionColor = makeRgb(0, 0, 0);
            free(restColor);
            double ratio = -dotProd; // both normal vectors
            double fresnel = 0.2 + 0.8 * pow(1 - ratio, 2); // TODO: make this better?
            Vec3* reflectDir = reflectVector(normalHit, ray -> dir, -ratio); // reflect ray across the normal vector
            // We want to move the origin of the ray slightly in the direction of the normal vector
            // So that recursive calls to trace don't continually hit the same point over and over again
            Vec3* rayOrigin = makeVec3(hitLocation -> x + normalHit -> x * SMALL, hitLocation -> y + normalHit -> y * SMALL, hitLocation -> z + normalHit -> z * SMALL);
            Ray* newRay = makeRayPointDir(rayOrigin, reflectDir);
            Rgb* reflectionColor = trace(newRay, spheres, sphereLength, depth + 1);
            if (reflectionColor == NULL) reflectionColor = makeRgb(255, 255, 255);

            if (hit -> refractivity > 0) {
                free(newRay -> dir);
                free(refractionColor);
                double d = hit -> refractionIndex;
                double index = inside ? d : 1/d; // TODO: adjust
                Vec3* refrDir = refractVector(ray -> dir, normalHit, index, dotProd);
                if (refrDir == NULL) {
                    refractionColor = makeRgb(0, 0, 0);
                } else {
                    normalize(refrDir);
                    setVec3(newRay -> from, hitLocation -> x - normalHit -> x * SMALL, hitLocation -> y - normalHit -> y * SMALL, hitLocation -> z - normalHit -> z * SMALL);
                    newRay -> dir = refrDir;
                    setAddVec3(newRay -> to, newRay -> from, refrDir);
                    refractionColor = trace(newRay, spheres, sphereLength, depth + 1);
                    if (refractionColor == NULL) refractionColor = makeRgb(255, 255, 255);
                }
            }

            scale(reflectionColor, fresnel * hit -> reflectivity);
            scale(refractionColor, (1 - fresnel) * hit -> refractivity);
            
            multiplyColors(refractionColor, baseColor);
            incColor(reflectionColor, refractionColor);
            free(reflectDir);
            free(rayOrigin);
            freeRay(newRay);
            free(newRay);
            free(refractionColor);
            free(normalHit);
            free(baseColor);
            free(hitLocation);
            return reflectionColor;
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
        double d = -dot(normalHit, lightRay -> dir);
        
        scale(baseColor, shadow == 1 ? 0.0 : fmax(0.0, d));
        incColor(baseColor, restColor);
        free(restColor);
        free(hitLocation);
        freeRay(lightRay);
        free(lightRay);
        free(normalHit);
        return baseColor;
    }
}

double toRads(double deg) {
    return deg * PI / 180;
}

// Takes an integer in [0, maxval] and returns an integer in [-maxval/2, maxval/2]
static int16_t calcCoord(uint16_t coord, uint16_t value) {
    int16_t offset = (coord / 2 - value);
    return offset;
}

static ThreadArgs* makeThreadArgs(Vec3* r1, Vec3* r2, Sphere** spheres, int sphereLength, unsigned char* pixels, uint16_t row) {
    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args -> step1 = r1;
    args -> step2 = r2;
    args -> spheres = spheres;
    args -> sphereLength = sphereLength;
    args -> pixels = pixels;
    args -> row = row;
    return args;
}