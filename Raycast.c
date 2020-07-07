#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>
#include "Raycast.h"
#include "geometry/Ray.h"
#include "Consts.h"
#include "geometry/TriangleMesh.h"
#include "geometry/SurfaceHit.h"

extern uint16_t width;
extern uint16_t height;
extern double aspectRatio;
extern double vFOV;
extern Vec3 cameraLocation;
extern Vec3 cameraDirection;
extern double cameraOrientation;
extern Vec3 light;
extern Rgb lightColor;

static double toRads(double deg);
static int16_t calcCoord(uint16_t, uint16_t);
static Rgb* trace(Ray*, Wrapper*, int);
static ThreadArgs* makeThreadArgs(Vec3*, Vec3*, Wrapper*, unsigned char*, uint16_t);
static void* threadCall(void*);

void raycast(FILE* file, Wrapper* list) {
    normalize(&cameraDirection);
    // The camera direction uniquely defines a plane that will represent the
    // viewpoint of the renderer.

    // In order to find a point on the above plane to draw a ray through, we
    // need two vectors in order to make an orthonormal basis of the plane
    Vec3** temp = getOrthogonalVectors(&cameraDirection);
    Vec3* v1 = temp[0];
    Vec3* v2 = temp[1];

    // We then rotate these two basis vectors around the cameraDirection to
    // account for the orientation of the camera
    Vec3* r1 = rotate(v1, &cameraDirection, cameraOrientation - PI / 4);
    Vec3* r2 = rotate(v2, &cameraDirection, cameraOrientation - PI / 4);

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
        ThreadArgs* args = makeThreadArgs(r1, r2, list, pixels, i);
        fullArgs[i] = args;

        // create the thread
        #ifdef MULTITHREAD
            if (pthread_create(&ids[i], NULL, threadCall, (void*) args) != 0) {
                printf("Something went wrong: %"PRIu16"\n", i);
                return;
            }
        #else
            threadCall((void*)args);
        #endif
    }

    // make sure that all of the threads finish execution before writing pixel data to file
    #ifdef MULTITHREAD
        for (uint16_t i = 0; i < height; i++) {
            pthread_join(ids[i], NULL);
        }
    #endif

    fwrite(pixels, sizeof(unsigned char), 3 * width * height, file);
    free(pixels);
    free(ids);

    for (int i = 0; i < height; i++) free(fullArgs[i]);
    free(fullArgs);
    free(v1);
    free(v2);
    free(r1);
    free(r2);
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
    
    for (uint16_t j = 0; j < width; j++) {
        if (j % 20 == 0) printf("Tracing %"PRIu16", %"PRIu16"\n", i, j);
        incVec3(scaledV2, step);
        setAddVec3(dir, temp, scaledV2);
        normalize(dir);
        ray -> dir = dir;
        Rgb* color = trace(ray, arguments -> spheres, 0);
        if (color == NULL) color = makeRgb(135, 206, 235); // sky color
        arguments -> pixels[index++] = color -> b;
        arguments -> pixels[index++] = color -> g;
        arguments -> pixels[index++] = color -> r;
        free(color);
    }

    freeRay(ray);
    free(ray);
    free(scaledV1);
    free(scaledV2);
    free(temp);
    free(step);
    return NULL;
}

static Rgb* trace(Ray* ray, Wrapper* list, int depth) {
    Wrapper* head = list;
    double minDistance = INFTY;
    SurfaceHit* surfaceHit = NULL;
    Wrapper* hit = NULL;
    Vec3 normalHit = {INFTY, INFTY, INFTY, 0.0};
    int index = 0;
    int i = -1;
    // find which object the ray hits
    while (head != NULL) {
        i++;
        double before = minDistance;
        Vec3 normal;
        SurfaceHit* temp = NULL;
        switch(head -> type) {
            case SPHERE: ; // empty statement to satisfy the compiler
                normal = sphereIntersect((Sphere*) (head -> ptr), ray, &minDistance);
                if (normal.x == INFTY) { // normalized, only one check is necessary
                    head = head -> next;
                    continue;
                }
                break;
            case MESH: ;
                temp = meshIntersect((Mesh*)(head -> ptr), ray, &minDistance);
                if (temp == NULL) {
                    head = head -> next;
                    continue;
                }
                break;
            default:
                printf("ERROR: Unexpected wrapper type %d found\n", head -> type);
                return NULL;
        }
        if (before != minDistance) {
            if (surfaceHit != NULL) free(surfaceHit);
            surfaceHit = temp;
            hit = head;
            index = i;
        }
        head = head -> next;
    }

    if (surfaceHit == NULL) {
        return NULL;
    } else { // hit
        normalHit = *surfaceHit -> n;
        normalize(&normalHit);
        Vec3* hitLocation = surfaceHit ->  p;
        Rgb* baseColor = surfaceHit -> color;
        double refractivity = 0.0, reflectivity = 0.0, refractionIndex = 0.0;

        // get surface data depending on type
        switch(hit -> type) {
            case SPHERE:
                refractivity = ((Sphere*)(hit -> ptr)) -> refractivity;
                reflectivity = ((Sphere*)(hit -> ptr)) -> reflectivity;
                refractionIndex = ((Sphere*)(hit -> ptr)) -> refractionIndex;
                break;
            case MESH:
                refractivity = ((Mesh*)(hit -> ptr)) -> refractivity;
                reflectivity = ((Mesh*)(hit -> ptr)) -> reflectivity;
                refractionIndex = ((Mesh*)(hit -> ptr)) -> refractionIndex;
                break;
            default:
                break;
        }

        adjustObjNormal(hit, hitLocation, &normalHit);
        multiplyColors(baseColor, &lightColor);

        int inside = 0;
        double dotProd = dot(ray -> dir, &normalHit);

        if (dotProd > 0) {
            inside = 1;
            scaleVec3(&normalHit, -1);
            dotProd *= -1;
        }

        if ((reflectivity > 0 || refractivity > 0) && depth < MAX_DEPTH) {
            Rgb* refractionColor = makeRgb(0, 0, 0);
            double ratio = -dotProd; // both normal vectors
            double fresnel = 0.2 + 0.8 * (1 - ratio) * (1 - ratio); // TODO: make this better?
            Vec3* reflectDir = reflectVector(&normalHit, ray -> dir, dotProd); // reflect ray across the normal vector
            // We want to move the origin of the ray slightly in the direction of the normal vector
            // So that recursive calls to trace don't continually hit the same point over and over again
            Vec3 rayOrigin = {hitLocation -> x + normalHit.x * SMALL, hitLocation -> y + normalHit.y * SMALL, hitLocation -> z + normalHit.z * SMALL, 0.0};

            Ray* newRay = makeRayPointDir(&rayOrigin, reflectDir);
            Rgb* reflectionColor = trace(newRay, list, depth + 1);
            if (reflectionColor == NULL) reflectionColor = makeRgb(255, 255, 255);

            if (refractivity > 0) {
                free(refractionColor);
                double index = inside ? refractionIndex : 1 / refractionIndex;
                Vec3* refrDir = refractVector(ray -> dir, &normalHit, index, dotProd);
                if (refrDir == NULL) {
                    refractionColor = makeRgb(0, 0, 0);
                } else {
                    normalize(refrDir);
                    setVec3(newRay -> from, hitLocation -> x - normalHit.x * SMALL, hitLocation -> y - normalHit.y * SMALL, hitLocation -> z - normalHit.z * SMALL);
                    copyVec3(newRay -> dir, refrDir);
                    refractionColor = trace(newRay, list, depth + 1);
                    if (refractionColor == NULL) refractionColor = makeRgb(135, 206, 235); // sky color
                    free(refrDir);
                }
            }

            scale(reflectionColor, fresnel * reflectivity * 0.5);
            scale(refractionColor, (1 - fresnel) * refractivity * 0.5);
            
            incColor(reflectionColor, refractionColor);
            multiplyColors(reflectionColor, baseColor);
            free(reflectDir);
            freeRay(newRay);
            free(newRay);
            free(refractionColor);
            free(baseColor);
            free(hitLocation);
            return reflectionColor;
        }

        // not reflective or refractive

        Ray* lightRay = makeRay(&light, hitLocation);
        double distSquared = distance2(&light, hitLocation);
        double dummy = INFTY;
        int shadow = 0;
        // Check all other objects to see if there is a shadow
        head = list;
        i = -1;
        // while (head != NULL) { // TODO: Make this loop work
        //     i++;
        //     if (i == index) {
        //         head = head -> next;
        //         continue;
        //     }
        //     Vec3 normal = sphereIntersect((Sphere*) (head -> ptr), lightRay, &dummy);
        //     if (normal.x != INFTY && dummy * dummy < distSquared) {
        //         // ensure intersection falls between light and hitlocation
        //         shadow = 1;
        //         dummy = INFTY;
        //         break;
        //     }
        //     head = head -> next;
        // }
        
        double d = -dot(&normalHit, lightRay -> dir);

        // if (d < 0) printf("BAD\n");
        
        scale(baseColor, shadow == 1 ? 0.0 : fmax(0.0, d)); // TODO: Brightness decrease with distance
        free(hitLocation);
        freeRay(lightRay);
        free(lightRay);
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

static ThreadArgs* makeThreadArgs(Vec3* r1, Vec3* r2, Wrapper* spheres, unsigned char* pixels, uint16_t row) {
    ThreadArgs* args = malloc(sizeof(ThreadArgs));
    args -> step1 = r1;
    args -> step2 = r2;
    args -> spheres = spheres;
    args -> pixels = pixels;
    args -> row = row;
    return args;
}