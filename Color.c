#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Color.h"

static double hueToRgb(double, double, double);

Rgb* makeRgb(unsigned char r, unsigned char g, unsigned char b) {
    Rgb* color = malloc(sizeof(Rgb));
    color -> r = r;
    color -> g = g;
    color -> b = b;
    return color;
}

Rgb* hslToRgb(Hsl* hsl) {
    double h = (hsl -> h);
    double s = (hsl -> s);
    double l = (hsl -> l);
    double r = 0, g = 0, b = 0;

    if (s == 0) {
        r = l;
        g = l;
        b = l;
    } else {
        double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        double p = 2 * l - q;
        r = hueToRgb(p, q, h + 1/3.0);
        g = hueToRgb(p, q, h);
        b = hueToRgb(p, q, h - 1/3.0);
    }

    Rgb* rgb = malloc(sizeof(Rgb));
    rgb -> r = (unsigned char)fmin(255.0, (r * 256));
    rgb -> g = (unsigned char)fmin(255.0, (g * 256));
    rgb -> b = (unsigned char)fmin(255.0, (b * 256));
    return rgb;
    
}

Hsl* rgbToHsl(Rgb* rgb) {
    double delta, min;
    double h=0, s=0, v=0;

    min = fmin(fmin(rgb -> r, rgb -> g), rgb -> b);
    v = fmax(fmax(rgb -> r, rgb -> g), rgb -> b);
    delta = v - min;

    if (v == 0.0) {
        s = 0;
    } else {
        s = delta / v;
    }

    if (s == 0) {
        h = 0.0;
    } else {
        if (rgb -> r == v) {
            h = (rgb -> g - rgb -> b) / delta;
        } else if (rgb -> g == v) {
            h = 2 + (rgb -> b - rgb -> r) / delta;
        } else if (rgb -> b == v) {
            h = 4 + (rgb -> r - rgb -> g) / delta;
        }

        h *= 60;
        if (h < 0.0) h += 360;
    }
    h /= 360.0;
    v /= 255.0;

    double l = (2 - s) * v / 2;
    if (l != 0) {
        if (l == 1) {
            s = 0;
        } else if (l < 0.5) {
            s = s * v / (l * 2);
        } else {
            s = s * v / (2 - l * 2);
        }
    }

    Hsl* hsl = malloc(sizeof(Hsl));
    hsl -> h = h;
    hsl -> s = s;
    hsl -> l = l;

    return hsl;
}

double hueToRgb(double p, double q, double t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0/6) return p + (q - p) * 6 * t;
    if (t < 1.0/2) return q;
    if (t < 2.0/3) return p + (q - p) * (2.0 / 3 - t) * 6;
    return p;
}

void scale(Rgb* rgb, double d) {
    rgb -> r *= d;
    rgb -> g *= d;
    rgb -> b *= d;
}

void printColor(Rgb* color) {
    printf("%d, %d, %d\n", color -> r, color -> g, color -> b);
}