#ifndef CONSTS_H
#define CONSTS_H
#include <math.h>

#define width 1920
#define height 1080
#define aspectRatio (width * 1.0 / height)
#define hFOV 90.0
#define PI 3.141592653589793
#define vFOV atan(tan(hFOV * PI / 360.0) * 1.0 / aspectRatio) * 360 / PI


#endif