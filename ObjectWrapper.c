#include <stdlib.h>
#include "geometry/Sphere.h"
#include "ObjectWrapper.h"
#include "Consts.h"
#include "geometry/TriangleMesh.h"

/**
 * Free all the pointers contained in the list
 */
void freeList(Wrapper* head) {
    if (head == NULL) return;
    freeList(head -> next);
    switch (head -> type) {
        case SPHERE:
            freeSphere(head -> ptr);
            break;
        case MESH: // TODO
            break;
    }
    free(head);
}

Wrapper* makeWrapper() {
    Wrapper* t = malloc(sizeof(Wrapper));
    t -> type = -1;
    t -> ptr = NULL;
    t -> next = NULL;
    return t;
}

Rgb* getObjColor(Wrapper* wrapper, Vec3* vec) {
    switch (wrapper -> type) {
        case SPHERE:
            return getSpherePixel((Sphere*)(wrapper -> ptr), vec);
        case MESH: // TODO
            return makeRgb(255, 0, 0);
    }
    return NULL;
}

void adjustObjNormal(Wrapper* wrapper, Vec3* loc, Vec3* normal) {
    switch (wrapper -> type) {
        case SPHERE:
            adjustSphereNormal((Sphere*)(wrapper -> ptr), loc, normal);
            break;
        case MESH: // TODO
            break;
    }
}