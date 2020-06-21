#include <stdlib.h>
#include <math.h>
#include "Quaternion.h"
#include "../Consts.h"

Quaternion* anglesToQuaternion(Angles* a) {
    double yaw = a -> z;
    double pitch = a -> y;
    double roll = a -> x;

    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    Quaternion* q = malloc(sizeof(Quaternion));

    q -> w = cr * cp * cy + sr * sp * sy;
    q -> x = sr * cp * cy - cr * sp * sy;
    q -> y = cr * sp * cy + sr * cp * sy;
    q -> z = cr * cp * sy - sr * sp * cy;

    q -> vec = makeVec3(q -> x, q -> y, q -> z);

    return q;
}

void freeQuaternion(Quaternion* q) {
    free(q -> vec);
}

Angles* quaternionToAngle(Quaternion* q) {
    Angles* angles = malloc(sizeof(Angles));

    double sinr_cosp = 2 * (q -> w * q -> x + q -> y * q -> z);
    double cosr_cosp = 1 - 2 * (q -> x * q -> x + q -> y * q -> y);

    angles -> x = atan2(sinr_cosp, cosr_cosp);

    double sinp = 2 * (q -> w * q -> y - q -> z * q -> x);
    if (abs(sinp) >= 1.0) angles -> y = sinp < 0 ? - PI / 2 : PI / 2;
    else angles -> y = asin(sinp);

    double siny_cosp = 2 * (q -> w * q -> z + q -> x * q -> y);
    double cosy_cosp = 1 - 2 * (q -> y * q -> y + q -> z * q -> z);
    angles -> z = atan2(siny_cosp, cosy_cosp);

    return angles;
}

Quaternion* makeQuaternion(double w, double x, double y, double z) {
    Quaternion* q = malloc(sizeof(Quaternion));
    q -> w = w;
    q -> x = x;
    q -> y = y;
    q -> z = z;
    q -> vec = makeVec3(x, y, z);
    return q;
}

Angles* makeAngles(double x, double y, double z) {
    Angles* a = malloc(sizeof(Angles));
    a -> x = x;
    a -> y = y;
    a -> z = z;
    return a;
}
Vec3* multiply(Quaternion* q, Vec3* vec) {
    // optimization from wikipedia
    Vec3* copy = copyScaleVec3(q -> vec, 2);
    Vec3* t = cross(copy, vec);
    Vec3* scaleT = copyScaleVec3(t, q -> w);
    Vec3* c = cross(q -> vec, t);
    incVec3(scaleT, c);
    incVec3(scaleT, vec);
    free(copy);
    free(t);
    free(c);
    return scaleT;
}

Quaternion* copyScaleQ(Quaternion* q, double d) {
    return makeQuaternion(q -> w * d, q -> x * d, q -> y * d, q -> z * d);
}