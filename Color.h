#ifndef COLOR_H
#define COLOR_H

struct _Rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};


typedef struct _Rgb Rgb;

Rgb* makeRgb(unsigned char, unsigned char, unsigned char);
void scale(Rgb*, double);
void printColor(Rgb*);
Rgb* addRgb(Rgb*, Rgb*);
int isWhite(Rgb*);
void multiplyColors(Rgb*, Rgb*);
void incColor(Rgb*, Rgb*);

#endif