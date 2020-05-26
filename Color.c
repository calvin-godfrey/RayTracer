#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Color.h"


Rgb* makeRgb(unsigned char r, unsigned char g, unsigned char b) {
    Rgb* color = malloc(sizeof(Rgb));
    color -> r = r;
    color -> g = g;
    color -> b = b;
    return color;
}

void scale(Rgb* rgb, double d) {
    rgb -> r *= d;
    rgb -> g *= d;
    rgb -> b *= d;
}

void printColor(Rgb* color) {
    printf("%d, %d, %d\n", color -> r, color -> g, color -> b);
}

Rgb* addRgb(Rgb* a, Rgb* b) {
    unsigned char red = a -> r + b -> r > 255 ? 255 : a -> r + b -> r;
    unsigned char green = a -> g + b -> g > 255 ? 255 : a -> g + b -> g;
    unsigned char blue = a -> b + b -> b > 255 ? 255 : a -> b + b -> b;
    return makeRgb(red, green, blue);
}

int isWhite(Rgb* c) {
    return c -> r == 255 && c -> g == 255 && c -> b == 255;
}

void multiplyColors(Rgb* a, Rgb* b) {
    a -> r = (unsigned char) (a -> r * (b -> r / 255.0));
    a -> b = (unsigned char) (a -> b * (b -> b / 255.0));
    a -> g = (unsigned char) (a -> g * (b -> g / 255.0));
}

void incColor(Rgb* a, Rgb* b) {
    a -> r += b -> r;
    a -> g += b -> g;
    a -> b += b -> b;
}