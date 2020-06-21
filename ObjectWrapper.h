#ifndef OBJECTWRAPPER_H
#define OBJECTWRAPPER_H
#include "Color.h"
#include "math/Vec3.h"

struct _ObjectWrapper {
    int type;
    void* ptr;
    struct _ObjectWrapper* next;
};

typedef struct _ObjectWrapper Wrapper;

void freeList(Wrapper*);
Wrapper* makeWrapper();
Rgb* getObjColor(Wrapper*, Vec3*);
void adjustObjNormal(Wrapper*, Vec3*, Vec3*);

#endif