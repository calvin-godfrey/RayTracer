#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <inttypes.h>
#include "../geometry/TriangleMesh.h"
#include "../geometry/Triangle.h"
#include "../geometry/Material.h"

int parseInput(FILE*, char*);
void writeHeader(FILE*);
Triangle** parseMesh(char*, Mesh*);
char* getFirstToken(char*);
void parseMtl(char*, Material**, int*);

#endif