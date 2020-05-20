#ifndef COLOR_H
#define COLOR_H

struct _Rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct _Hsv {
    double h;
    double s;
    double v;
};

struct _Hsl {
    double h;
    double s;
    double l;
};

typedef struct _Hsv Hsv;
typedef struct _Rgb Rgb;
typedef struct _Hsl Hsl;

Rgb* makeRgb(unsigned char, unsigned char, unsigned char);
Rgb* hslToRgb(Hsl*);
Hsl* rgbToHsl(Rgb*);
void scale(Rgb*, double);
void printColor(Rgb*);
Rgb* addRgb(Rgb*, Rgb*);
int isWhite(Rgb*);
void multiplyColors(Rgb*, Rgb*);

#endif