#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Raycast.h"
#include "Color.h"
#include "Sphere.h"
#include "Vec3.h"
#include "Consts.h"
#include "Texture.h"

uint16_t width;
uint16_t height;
double aspectRatio;
double vFOV;
Vec3 camera = {-11, 0, 0};
Vec3 light = {50, 0, 100};

static void writeHeader(FILE* file) {
    // 800x600 image, 24 bits per pixel
    unsigned char header[18] = {
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(width & 0x00FF), (unsigned char)((width & 0xFF00) / 256),
        (unsigned char)(height & 0x00FF), (unsigned char)((height & 0xFF00) / 256),
        0x18, 0x20 //0x18 is 24 bits per pixel, 0x20 means that the data for the first pixel goes in the top left corner
    };
    fwrite(header, sizeof(char), 18, file);
}

static void setGlobalVariables(char** argv) {
    sscanf(argv[1], "%"SCNu16, &width);
    sscanf(argv[2], "%"SCNu16, &height);
    aspectRatio = (width * 1.0) / height;
    vFOV = atan(tan(hFOV * PI / 360.0) * 1.0 / aspectRatio) * 360 / PI;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage:\n");
        printf("    raycaster <width> <height> <outputfile>\n");
        printf("    e.g. raycaster 640 480 file.tga\n");
        printf("    Note that the image generated uses the tga format\n");
        return 1;
    } else if (argc != 4) {
        printf("Expected 4 arguments\n");
        return 1;
    }
    srand(time(0));
    setGlobalVariables(argv);
    Rgb* red = makeRgb(255, 0, 0);
    Rgb* green = makeRgb(0, 255, 0);
    Rgb* blue = makeRgb(0, 0, 255);
    Rgb* gray = makeRgb(200, 200, 200);
    // Texture* t = makeTexture("../data/map.tga");
    // int size = 15;
    // Sphere** spheres = malloc(sizeof(Sphere*) * size);
    // for (int i = 0; i < size; i++) {
    //     int color = (rand() % 3);
    //     double radius = (double)((rand() % 3) + (rand() % 2) / 2.0);
    //     Vec3* center = makeVec3((double)(rand() % 10) - 5, (double)(rand() % 10) - 5, (double)(rand() % 10) - 5);
    //     spheres[i] = makeSphere(center, radius, color == 0 ? red : color == 1 ? green : blue);
    // }
    for (int i = 0; i < 360; i++) {

        Vec3 center1 = {2.8 + 9 * cos(i * PI / 180.0), 0.0 + 9 * sin(i * PI / 180.0), -0.0};
        Vec3 center2 = {2.8, -0.0, -1.0};
        Vec3 center3 = {10.0, 0.0, -10001.0 - center2.z};
        // Vec3 center3 = {3.0, -2.4, -4.0};
        Sphere* sphere1 = makeSphere(&center1, 0.8, red, NULL, 0.0, 0.0, 0.0);
        Sphere* sphere2 = makeSphere(&center2, 5.8, green, NULL, 1.0, 1.0, -i * PI / 180.0);
        Sphere* sphere3 = makeSphere(&center3, 10000.0 - sphere2 -> radius, gray, NULL, 0.0, 0.0, 0.0);
        // Sphere* sphere3 = makeSphere(&center3, 2.0, blue);
        Sphere* spheres[3] = {sphere1, sphere2, sphere3};
        char* fileName = calloc(100, sizeof(char));
        sprintf(fileName, "%s%03d.tga", argv[3], i);
        printf("%s\n", fileName);
        FILE* outFile = fopen(fileName, "w+");
        writeHeader(outFile);
        raycast(outFile, spheres, 3);
        fclose(outFile);
    }
    // for (int i = 0; i < size; i++) {
    //     freeSphere(spheres[i]);
    //     free(spheres[i]);
    // }
    // free(spheres);
    free(red);
    free(green);
    free(blue);
}