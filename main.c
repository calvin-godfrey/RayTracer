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
#include "Quaternion.h"

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
static int setCamera(char*);
static int setLight(char*);
static char* getFirstToken(char*);

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
    sscanf(argv[2], "%"SCNu16, &width);
    sscanf(argv[3], "%"SCNu16, &height);
    aspectRatio = (width * 1.0) / height;
    vFOV = atan(tan(hFOV * PI / 360.0) * 1.0 / aspectRatio) * 360 / PI;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("Usage:\n");
        printf("    raycaster <description file> <width> <height> <outputfile prefix>\n");
        printf("    e.g. raycaster 640 480 file.tga\n");
        printf("    Note that the image generated uses the tga format\n");
        return 1;
    } else if (argc != 5) {
        printf("Expected 5 arguments\n");
        return 1;
    }
    srand(time(0));
    setGlobalVariables(argv);

    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Failure to open file %s\n", argv[1]);
        return 1;
    }

    if (parseInput(input, argv[4])) {
        printf("Something went wrong\n");
        return 1;
    }
    return 0;
}

int parseInput(FILE* fp, char* out) {
    char line[LINE_LENGTH];
    if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
    int camFlag, lightFlag;
    if (sscanf(line, "%d %d %d", &frames, &camFlag, &lightFlag) != 3) return 1;
    if (camFlag) {
        if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
        if (setCamera(line)) return 1;
    }
    if (lightFlag) {
        if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
        if (setLight(line)) return 1;
    }
    for (int i = 0; i < frames; i++) {
        char* fileName = calloc(100, sizeof(char));
        sprintf(fileName, "%s%03d.tga", out, i);
        printf("Preparing for %s\n", fileName);
        FILE* outFile = fopen(fileName, "w+");
        writeHeader(outFile);

        if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
        int n;
        if (sscanf(line, "%d", &n) != 1) return 1;
        if (!camFlag) {
            if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
            if (setCamera(line)) return 1;
        }
        if (!lightFlag) {
            if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
            if (setLight(line)) return 1;
        }
        Sphere* head = NULL;
        for (int j = 0; j < n; j++) {
            if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
            char type[LINE_LENGTH];
            if (sscanf(line, "%s", type) != 1) return 1;
            if (strcmp(type, "SPHERE") == 0) {
                double x, y, z, r;
                double rx = 0;
                double ry = 0;
                double rz = 0;
                double index = 0;
                double refraction = 0;
                double reflection = 0;
                Rgb* color = NULL;
                Texture* texture = NULL;
                Texture* map = NULL;
                if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
                if (sscanf(line, "%lf %lf %lf %lf", &x, &y, &z, &r) != 4) return 1;
                Vec3* center = makeVec3(x, y, z);
                while (1) {
                    if (fgets(line, LINE_LENGTH, fp) == NULL) return 1;
                    char* token = getFirstToken(line);
                    if (strcmp(token, "DONE") == 0) {
                        free(token);
                        break;
                    } else if (strcmp(token, "TEXTURE") == 0) {
                        char* path = calloc(LINE_LENGTH, sizeof(char));
                        if (sscanf(line + strlen(token) + 1, "%s", path) != 1) return 1;
                        texture = makeTexture(path);
                        free(path);
                    } else if (strcmp(token, "COLOR") == 0) {
                        unsigned char r, g, b;
                        if (sscanf(line + strlen(token) + 1, "%hhu %hhu %hhu", &r, &g, &b) != 3) return 1;
                        color = makeRgb(r, g, b);
                    } else if (strcmp(token, "ROTATION") == 0) {
                        if (sscanf(line + strlen(token) + 1, "%lf %lf %lf", &rx, &ry, &rz) != 3) return 1;
                    } else if (strcmp(token, "INDEX") == 0) {
                        if (sscanf(line + strlen(token) + 1, "%lf", &index) != 1) return 1;
                    } else if (strcmp(token, "REFLECTIVITY") == 0) {
                        if (sscanf(line + strlen(token) + 1, "%lf", &reflection) != 1) return 1;
                    } else if (strcmp(token, "REFRACTIVITY") == 0) {
                        if (sscanf(line + strlen(token) + 1, "%lf", &refraction) != 1) return 1;
                    } else if (strcmp(token, "NORMAL") == 0) {
                        char* path = calloc(LINE_LENGTH, sizeof(char));
                        if (sscanf(line + strlen(token) + 1, "%s", path) != 1) return 1;
                        map = makeTexture(path);
                        free(path);
                    }
                    free(token);
                }
                Sphere* sphere = makeSphereRotation(center, r, color, texture, map, reflection, refraction, index, rx, ry, rz);
                if (head == NULL) {
                    head = sphere;
                } else {
                    head = insertSphere(head, sphere);
                }
            }
        }
        printf("Writing to %s\n", fileName);
        raycast(outFile, head);
        free(fileName);
        freeSpheres(head);
        fclose(outFile);
    }
    return 0;
}

static int setCamera(char* line) {
    double cx, cy, cz, dx, dy, dz, dt;
    if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf\n", &cx, &cy, &cz, &dx, &dy, &dz, &dt) != 7) return 1;
    cameraLocation.x = cx;
    cameraLocation.y = cy;
    cameraLocation.z = cz;
    cameraDirection.x = dx;
    cameraDirection.y = dy;
    cameraDirection.z = dz;
    cameraDirection.mag2 = dx * dx + dy * dy + dz * dz;
    cameraOrientation = dt * PI / 180;
    return 0;
}

static int setLight(char* line) {
    double cx, cy, cz;
    unsigned char r, g, b;
    if (sscanf(line, "%lf %lf %lf %hhu %hhu %hhu\n", &cx, &cy, &cz, &r, &g, &b) != 6) return 1;
    light.x = cx;
    light.y = cy;
    light.z = cz;
    lightColor.r = r;
    lightColor.g = g;
    lightColor.b = b;
    return 0;
}

static char* getFirstToken(char* line) {
    char* token = calloc(LINE_LENGTH, sizeof(char));
    int i = 0;
    while (line[i] != ' ' && line[i] != '\n') i++;
    strncpy(token, line, i);
    return token;
}