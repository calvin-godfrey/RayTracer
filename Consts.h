#ifndef CONSTS_H
#define CONSTS_H
#include <math.h>

#define aspectRatio (4.0 / 3.0)
#define hFOV 90.0
#define PI 3.141592653589793
#define vFOV atan(tan(hFOV * PI / 360.0) * 1.0 / aspectRatio) * 360 / PI


#endif