#include <stdlib.h>
#include <string.h>
#include "Parser.h"
#include "../math/Vec3.h"
#include "../Color.h"
#include "../geometry/Sphere.h"
#include "../ObjectWrapper.h"
#include "../Consts.h"
#include "../Texture.h"
#include "../Raycast.h"

extern Vec3 cameraLocation, cameraDirection;
extern int cameraOrientation;
extern int frames;
extern Vec3 light;
extern Rgb lightColor;
extern uint16_t width, height;

static int setCamera(char*);
static int setLight(char*);
static void parseVertex(char*, int*, int*, int*);

Triangle** parseMesh(char* path, Mesh* mesh) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", path);
        return NULL;
    }
    // we first have to count everything before we can malloc memory
    char* line = calloc(LINE_LENGTH, sizeof(char));
    int numV = 0, numVt = 0, numVn = 0, numTri = 0;
    while (1) {
        if (fgets(line, LINE_LENGTH, file) == NULL) break;
        char* token = getFirstToken(line);
        if (strcmp(token, "mtllib") == 0) {
            free(token);
            continue; // TODO: allow parse of mtl file
        } else if (strcmp(token, "v") == 0) {
            numV++;
        } else if (strcmp(token, "vp") == 0) {
            printf("Error: File contains unexpected line vp\n");
            return NULL;
        } else if (strcmp(token, "vn") == 0) {
            numVn++;
        } else if (strcmp(token, "vt") == 0) {
            numVt++;
        } else if (strcmp(token, "cstype") == 0) {
            printf("File of unexpected type. Failing\n");
            return NULL;
        } else if (strcmp(token, "f") == 0) {
            int count = 0;
            char* temp = calloc(20, 1);
            int index = 1;
            while (sscanf(line + strlen(token) + index, "%[0-9/]", temp) == 1) {
                index += strlen(temp) + 1;
                count++;
            }
            numTri += count - 2;
            free(temp);
        } else if (strcmp(token, "p") == 0) { // parse a polygon? Not sure
            printf("Cannot parse polygon yet.\n");
            return NULL;
        }
        // this is all that matters in the first pass
    }
    fseek(file, 0, SEEK_SET);
    // allocate memory for the arrays
    mesh -> nTriangles = numTri;
    mesh -> nVert = numV;
    Vec3* vArray = malloc(numV * sizeof(Vec3));
    Vec3* vnArray = malloc(numVn * sizeof(Vec3));
    Vec3* uvArray = malloc(numVt * sizeof(Vec3)); // only two components actually used
    int* meshIndices = malloc(numTri * 3 * sizeof(int));
    Triangle** triangleArray = calloc(numTri + 1, sizeof(Triangle*));
    numV = 0, numVt = 0, numVn = 0;
    int triangleIndex = 0;
    while (1) {
        if (fgets(line, LINE_LENGTH, file) == NULL) break;
        char* token = getFirstToken(line);
        if (strcmp(token, "v") == 0) {
            double x, y, z;
            if (sscanf(line, "v %lf %lf %lf", &x, &y, &z) != 3) {
                printf("Failed to parse object line %s", line);
                return NULL;
            }
            vArray[numV++] = (Vec3) {x, y, z, 0}; // mag component doesn't matter
        } else if (strcmp(token, "vn") == 0) {
            double x, y, z;
            if (sscanf(line, "vn %lf %lf %lf", &x, &y, &z) != 3) {
                printf("Failed to parse object line %s", line);
                return NULL;
            }
            vnArray[numVn++] = (Vec3) {x, y, z, x*x+y*y+z*z};
        } else if (strcmp(token, "vt") == 0) {
            double u, v;
            if (sscanf(line, "vt %lf %lf", &u, &v) != 2) {
                printf("Failed to parse object line %s", line);
                return NULL;
            }
            uvArray[numVt++] = (Vec3) {u, v, 0, 0};
        } else if (strcmp(token, "f") == 0) {
            int count = 0;
            char* temp = calloc(20, 1);
            int index = 1;
            while (sscanf(line + strlen(token) + index, "%[0-9/]", temp) == 1) {
                index += strlen(temp) + 1;
                count++;
            }
            count -= 2; // number of triangles
            char** arr = malloc(sizeof(char*) * (count + 2));
            index = 1;
            int i = 0;
            while (sscanf(line + strlen(token) + index, "%[0-9/]", temp) == 1) {
                index += strlen(temp) + 1;
                arr[i] = calloc(20, sizeof(char));
                strncpy(arr[i++], temp, 19);
            }
            for (int j = 1; j < count + 1; j++) {
                Triangle* triangle = malloc(sizeof(Triangle));
                triangle -> mesh = mesh;
                triangle -> vert = malloc(3 * sizeof(int));
                char* s1 = arr[0];
                char* s2 = arr[j];
                char* s3 = arr[j+1];
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                parseVertex(s1, &v1, &vt1, &vn1);
                parseVertex(s2, &v2, &vt2, &vn2);
                parseVertex(s3, &v3, &vt3, &vn3);
                triangle -> vert[0] = v1 < 0 ? numV - v1 + 1 : v1 - 1; // if vertex index is negative, go back from current
                triangle -> vert[1] = v2 < 0 ? numV - v2 + 1 : v2 - 1;
                triangle -> vert[2] = v3 < 0 ? numV - v3 + 1 : v3 - 1;
                meshIndices[3 * triangleIndex] = triangle -> vert[0];
                meshIndices[3 * triangleIndex + 1] = triangle -> vert[1];
                meshIndices[3 * triangleIndex + 2] = triangle -> vert[2];
                if (vt1 == BIG_INT) {
                    triangle -> uv = NULL;
                } else {
                    triangle -> uv = malloc(3 * sizeof(int));
                    triangle -> uv[0] = vt1 < 0 ? numVt - vt1 + 1 : vt1 - 1;
                    triangle -> uv[1] = vt2 < 0 ? numVt - vt2 + 1 : vt2 - 1;
                    triangle -> uv[2] = vt3 < 0 ? numVt - vt3 + 1 : vt3 - 1;
                }
                if (vn1 == BIG_INT) {
                    triangle -> norm = NULL;
                } else {
                    triangle -> norm = malloc(3 * sizeof(int));
                    triangle -> norm[0] = vn1 < 0 ? numVn - vn1 + 1 : vn1 - 1;
                    triangle -> norm[1] = vn2 < 0 ? numVn - vn2 + 1 : vn2 - 1;
                    triangle -> norm[2] = vn3 < 0 ? numVn - vn3 + 1 : vn3 - 1;
                }
                triangleArray[triangleIndex++] = triangle;
            }
            for (int j = 0; j < count + 2; j++) free(arr[j]);
            free(arr);
            free(temp);
        }
        free(token);
    }
    mesh -> p = vArray;
    mesh -> n = vnArray;
    mesh -> uv = uvArray;
    mesh -> vertInd = meshIndices;
    return triangleArray;
}

static void parseVertex(char* s, int* v, int* vt, int* vn) {
    if (sscanf(s, "%d/%d/%d", v, vt, vn) == 3) return;
    *vt = BIG_INT;
    if (sscanf(s, "%d//%d", v, vn) == 2) return;
    *vn = BIG_INT;
    if (sscanf(s, "%d", v) == 1) return;
    printf("Error parsing face vertex %s\n", s);
}

char* getFirstToken(char* line) {
    char* token = calloc(LINE_LENGTH, sizeof(char));
    int i = 0;
    while (line[i] != ' ' && line[i] != '\n') i++;
    strncpy(token, line, i);
    return token;
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
        Wrapper* head = makeWrapper();
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
                if (head -> type == -1) {
                    head -> type = SPHERE;
                    head -> ptr = sphere;
                } else {
                    Wrapper* temp = makeWrapper();
                    temp -> type = SPHERE;
                    temp -> ptr = sphere;
                    temp -> next = head;
                    head = temp;
                }
            }
        }
        printf("Writing to %s\n", fileName);
        raycast(outFile, head);
        free(fileName);
        freeList(head);
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

void writeHeader(FILE* file) {
    // 800x600 image, 24 bits per pixel
    unsigned char header[18] = {
        0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        (unsigned char)(width & 0x00FF), (unsigned char)((width & 0xFF00) / 256),
        (unsigned char)(height & 0x00FF), (unsigned char)((height & 0xFF00) / 256),
        0x18, 0x20 //0x18 is 24 bits per pixel, 0x20 means that the data for the first pixel goes in the top left corner
    };
    fwrite(header, sizeof(char), 18, file);
}