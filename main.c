#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Raycast.h"
#include "Color.h"
#include "geometry/Sphere.h"
#include "geometry/TriangleMesh.h"
#include "math/Vec3.h"
#include "Consts.h"
#include "Texture.h"
#include "math/Quaternion.h"
#include "math/Transform.h"
#include "math/Matrix4.h"
#include "ObjectWrapper.h"
#include "util/Parser.h"

uint16_t width;
uint16_t height;
double aspectRatio;
double vFOV;
Vec3 cameraLocation;
Vec3 cameraDirection;
double cameraOrientation = 0; // in radians
Vec3 light;
Rgb lightColor;
int frames;

int parseInput(FILE*, char* out);

static void setGlobalVariables(char** argv) {
    sscanf(argv[2], "%"SCNu16, &width);
    sscanf(argv[3], "%"SCNu16, &height);
    aspectRatio = (width * 1.0) / height;
    vFOV = atan(tan(hFOV * PI / 360.0) * 1.0 / aspectRatio) * 360 / PI;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage:\n");
        printf("    raycaster <description file> <width> <height> <outputfile prefix>\n");
        printf("    e.g. raycaster input.txt 640 480 file.tga\n");
        printf("    Note that the image generated uses the tga format\n");
        return 1;
    } else if (argc != 5) {
        printf("Expected 5 arguments\n");
        return 1;
    }
    srand(time(0));
    setGlobalVariables(argv);

    Mesh* mesh = malloc(sizeof(Mesh));
    Triangle** arr = parseMesh("data/obj.obj", mesh);
    double minX = INFTY, minY = INFTY, minZ = INFTY, maxX = -INFTY, maxY = -INFTY, maxZ = -INFTY;
    for (int i = 0; i < mesh -> nVert; i++) {
        Vec3 v = mesh -> p[i];
        minX = v.x < minX ? v.x : minX;
        minY = v.y < minY ? v.y : minY;
        minZ = v.z < minZ ? v.z : minZ;

        maxX = v.x > maxX ? v.x : maxX;
        maxY = v.y > maxY ? v.y : maxY;
        maxZ = v.z > maxZ ? v.z : maxZ;
    }
    Vec3* min = makeVec3(minX, minY, minZ);
    Vec3* max = makeVec3(maxX, maxY, maxX);
    Wrapper* head = makeWrapper();
    head -> ptr = mesh;
    head -> type = MESH;
    cameraLocation.x = 0;
    cameraLocation.y = 90;
    cameraLocation.z = 90;
    cameraDirection.x = 0;
    cameraDirection.y = -0.1;
    cameraDirection.z = -1;
    cameraDirection.mag2 = 1;
    normalize(&cameraDirection);
    cameraOrientation = 3 * PI / 4;
    light.x = 0;
    light.y = 100;
    light.z = 100;
    lightColor.r = (unsigned char) 255;
    lightColor.g = (unsigned char) 255;
    lightColor.b = (unsigned char) 255;

    mesh -> box = makeBoundingBox(min, max);
    char* fileName = calloc(100, sizeof(char));
    sprintf(fileName, "%s.tga", argv[4]);
    printf("Preparing for %s\n", fileName);
    FILE* outFile = fopen(fileName, "w+");
    writeHeader(outFile);

    raycast(outFile, head);

    // for (int i = 0; i < 360; i++) {

    //     char* fileName = calloc(100, sizeof(char));
    //     sprintf(fileName, "%s%03d.tga", argv[4], i);
    //     printf("Preparing for %s\n", fileName);
    //     FILE* outFile = fopen(fileName, "w+");
    //     writeHeader(outFile);

    //     cameraLocation.x = -2;
    //     cameraLocation.y = 0;
    //     cameraLocation.z = -1;
    //     cameraDirection.x = 1;
    //     cameraDirection.y = 0;
    //     cameraDirection.z = 0;
    //     cameraDirection.mag2 = 1;
    //     light.x = -12;
    //     light.y = 0;
    //     light.z = 0;
    //     lightColor.r = (unsigned char) 255;
    //     lightColor.g = (unsigned char) 255;
    //     lightColor.b = (unsigned char) 255;

    //     Wrapper* head = makeWrapper();
    //     Mesh* mesh = malloc(sizeof(Mesh));
    //     Matrix4 matrix = makeMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1); // identiy
    //     Transform t = makeTransform(&matrix);
    //     mesh -> toWorld = &t;
    //     mesh -> nTriangles = 1;
    //     mesh -> nVert = 3;
    //     mesh -> vertInd = malloc(3 * sizeof(int));
    //     mesh -> vertInd[0] = 0;
    //     mesh -> vertInd[1] = 1;
    //     mesh -> vertInd[2] = 2;
    //     mesh -> p = malloc(3 * sizeof(Vec3));
    //     mesh -> p[0] = (Vec3) {sin(i * PI / 180), 0, -1 + 2 * cos(i * PI / 180), 1};
    //     mesh -> p[1] = (Vec3) {0, -1, -1, sqrt(2)};
    //     mesh -> p[2] = (Vec3) {0, 1, -1, sqrt(2)};
    //     mesh -> n = NULL;
    //     Vec3* low = makeVec3(fmin(0, sin(i * PI / 180)), -1, fmin(-1, -1 + 2 * cos(i * PI / 180)));
    //     Vec3* high = makeVec3(fmax(0, sin(i * PI / 180)), 1, fmax(-1, -1 + 2 * cos(i * PI / 180)));
    //     BoundingBox* box = makeBoundingBox(low, high);
    //     mesh -> box = box;
    //     mesh -> reflectivity = 0.0;
    //     mesh -> refractivity = 0.0;
    //     mesh -> refractionIndex = 0.0;
    //     head -> ptr = mesh;
    //     head -> type = MESH;
    //     raycast(outFile, head);
    //     fclose(outFile);
    //     free(mesh -> p);
    //     free(mesh -> vertInd);
    //     free(mesh);
    //     free(low);
    //     free(high);
    //     free(head);
    // }


    // FILE* input = fopen(argv[1], "r");
    // if (input == NULL) {
    //     printf("Failure to open file %s\n", argv[1]);
    //     return 1;
    // }

    // if (parseInput(input, argv[4])) {
    //     printf("Something went wrong\n");
    //     return 1;
    // }
    return 0;
}