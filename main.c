#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include "Raycast.h"
#include "Color.h"
#include "Sphere.h"
#include "Vec3.h"
#include "Consts.h"

static void writeHeader(FILE* file) {
    // 800x600 image, 24 bits per pixel
    unsigned char header[18] = {
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(width & 0x00FF), (unsigned char)((width & 0xFF00) / 256),
        (unsigned char)(height & 0x00FF), (unsigned char)((height & 0xFF00) / 256),
        0x18, 0x00
    };
    fwrite(header, sizeof(char), 18, file);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments: expected file name\n");
        return 1;
    }
    srand(time(0));
    FILE* file = fopen(argv[1], "w+");
    writeHeader(file);
    Rgb* red = makeRgb(255, 0, 0);
    Rgb* green = makeRgb(0, 255, 0);
    Rgb* blue = makeRgb(0, 0, 255);
    // int size = 15;
    // Sphere** spheres = malloc(sizeof(Sphere*) * size);
    // for (int i = 0; i < size; i++) {
    //     int color = (rand() % 3);
    //     double radius = (double)((rand() % 3) + (rand() % 2) / 2.0);
    //     Vec3* center = makeVec3((double)(rand() % 10) - 5, (double)(rand() % 10) - 5, (double)(rand() % 10) - 5);
    //     spheres[i] = makeSphere(center, radius, color == 0 ? red : color == 1 ? green : blue);
    // }
    for (int i = 0; i < 360; i++) {

        Vec3 center1 = {8.0 + 5 * cos(i * PI / 180.0), 0.0 + 5 * sin(i * PI / 180.0), 0.0};
        Vec3 center2 = {8.0, -0.0, -0.0};
        // Vec3 center3 = {3.0, -2.4, -4.0};
        Sphere* sphere1 = makeSphere(&center1, 0.8, red);
        Sphere* sphere2 = makeSphere(&center2, 2.8, green);
        // Sphere* sphere3 = makeSphere(&center3, 2.0, blue);
        Sphere* spheres[2] = {sphere1, sphere2};
        char* fileName = calloc(100, sizeof(char));
        sprintf(fileName, "%s%03d.tga", argv[1], i);
        printf("%s\n", fileName);
        FILE* outFile = fopen(fileName, "w+");
        writeHeader(outFile);
        raycast(outFile, spheres, 2);
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
    fclose(file);
}