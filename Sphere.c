#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Sphere.h"
#include "Consts.h"


static double* getSphereCoordinates(Sphere*, Vec3*);

/**
 * Returns a pointer to a Sphere struct object. Note that in the logic used, it is assumed that
 * if a sphere is refractive (i.e., has some transparency), it is also reflective. Also note
 * that a sphere cannot have both a texture AND color. Texture has higher priority; if it is
 * null, only then is its color used
 */
Sphere* makeSphere(Vec3* center, double r, Rgb* c, Texture* t, double reflectivity, double refractivity, double refractionIndex) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    sphere -> reflectivity = reflectivity;
    sphere -> refractivity = refractivity;
    sphere -> texture = t;
    sphere -> refractionIndex = refractionIndex;
    Angles* temp = makeAngles(0, 0, 0);
    sphere -> rotation = anglesToQuaternion(temp);
    sphere -> next = NULL;
    free(temp);
    return sphere;
}

Sphere* makeSphereRotation(Vec3* center, double r, Rgb* c, Texture* t, Texture* n, double refl, double refrac, double index, double rx, double ry, double rz) {
    Sphere* sphere = malloc(sizeof(Sphere));
    sphere -> center = center;
    sphere -> radius = r;
    sphere -> color = c;
    sphere -> reflectivity = refl;
    sphere -> refractivity = refrac;
    sphere -> texture = t;
    sphere -> normalMap = n;
    sphere -> refractionIndex = index;
    Angles* temp = makeAngles(rx * PI / 180, ry * PI / 180, rz * PI / 180);
    sphere -> rotation = anglesToQuaternion(temp);
    sphere -> next = NULL;
    free(temp);
    return sphere;
}

void freeSphere(Sphere* sphere) {
    free(sphere -> center);
    freeTexture(sphere -> texture);
    freeTexture(sphere -> normalMap);
}

Vec3 sphereIntersect(Sphere* sphere, Ray* ray, double* minDistance) {
    Vec3 bad = {INFTY, INFTY, INFTY, 0};
    Vec3 L;
    setVec3(&L, sphere -> center -> x - ray -> from -> x,
                sphere -> center -> y - ray -> from -> y,
                sphere -> center -> z - ray -> from -> z);
    double tca = dot(&L, ray -> dir);
    double d2 = L.mag2 - tca * tca;
    double r2 = sphere -> radius * sphere -> radius;
    if (d2 > r2) { // no intersect
        return bad;
    } else {
        double thc = sqrt(r2 - d2);
        double x0 = tca - thc;
        double x1 = tca + thc;
        double min = fmin(x0, x1);
        double max = fmax(x0, x1);
        double ans = min > 0 ? min : max;
        if (ans < 0) {
            return bad;
        }
        if (ans < *minDistance) *minDistance = ans;
        Vec3* point = getPoint(ray, ans);
        Vec3 normal;
        setVec3(&normal, point -> x - sphere -> center -> x,
                         point -> y - sphere -> center -> y,
                         point -> z - sphere -> center -> z);
        free(point);
        return normal;
    }    
}

Rgb* getPixelData(Sphere* sphere, Vec3* point) {
    if (sphere -> texture == NULL) {
        return makeRgb(sphere -> color -> r, sphere -> color -> g, sphere -> color -> b);
    }
    double* arr = getSphereCoordinates(sphere, point);
    double x = arr[0];
    double y = arr[1];
    free(arr);
    return getPixel(sphere -> texture, x, y);
}

/**
 * Insert second parameter as the new head, replacing first
 */
Sphere* insertSphere(Sphere* head, Sphere* new) {
    new -> next = head;
    return new;
}

void freeSpheres(Sphere* head) {
    Sphere* next = head -> next;
    if (next != NULL) {
        freeSpheres(next);
    }
    freeSphere(head);
    free(head);
}

static double* getSphereCoordinates(Sphere* sphere, Vec3* point) {
    Vec3* coordinates = sub(point, sphere -> center);
    Vec3* rotated = multiply(sphere -> rotation, coordinates);

    double y = (rotated -> z / sphere -> radius + 1) / 2; // in range [0, 1]
    double angle = atan2(rotated -> y, rotated -> x); // in [-pi, pi]
    double x = (angle + PI) / (2 * PI) + 2; // transform [-pi, pi] to [0, 1], + 2 ensurse that it's positive
    x = x - (int) x;
    if (x == 1.0) x = 0.0; // edge case, texture only works on [0, 1)
    free(coordinates);
    free(rotated);
    double* ans = malloc(2 * sizeof(double));
    ans[0] = x;
    ans[1] = y;
    return ans;
}

/**
 * Adjusts the normal (3rd parameter) at point (2nd paramter) according to normal map
 */
void adjustNormal(Sphere* sphere, Vec3* point, Vec3* normal) {
    if (sphere -> normalMap == NULL) return;
    double* arr = getSphereCoordinates(sphere, point);
    double x = arr[0];
    double y = arr[1];
    free(arr);
    Rgb* color = getPixel(sphere -> normalMap, x, y);
    x = (color -> r - 127.5) / 127.5;
    y = (color -> g - 127.5) / 127.5;
    double z = (color -> b - 128) / 127.0;
    free(color);
    Vec3* ortho = getSphereTangent(sphere, point); // basis of tangent space
    Vec3 tangent = ortho[0];
    Vec3 bitangent = ortho[1];
    free(ortho);
    scaleVec3(normal, z);
    scaleVec3(&tangent, y);
    scaleVec3(&bitangent, x);
    setAddVec3(normal, normal, &tangent);
    setAddVec3(normal, normal, &bitangent);
    normalize(normal);
}

Vec3* getSphereTangent(Sphere* sphere, Vec3* point) {
    double* arr = getSphereCoordinates(sphere, point);
    double theta = arr[1];
    double z = point -> z;
    // use geometry to avoid expensive trig functions
    double zr = sqrt(point -> x * point -> x + point -> y * point -> y);
    double invZr = 1/zr;
    double cosPhi = point -> x * invZr;
    double sinPhi = point -> y * invZr;

    Vec3 dpdu = {-point -> y, point -> x, 0, zr*zr};
    Vec3 dpdv = {z * cosPhi, z * sinPhi, -sphere -> radius * sin(theta), 0};
    setMag(&dpdv);
    normalize(&dpdu);
    normalize(&dpdv);
    Vec3* ans = malloc(sizeof(Vec3) * 2);
    ans[0] = dpdu;
    ans[1] = dpdv;
    return ans;
}