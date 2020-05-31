SHELL=/bin/bash
CC=gcc
CFLAGS=-std=c11 -Wall -W -ggdb3 -O3 -lm -lpthread

raycaster: main.c Color.c Sphere.c Vec3.c Ray.c Raycast.c Consts.h Texture.c Quaternion.c BoundingBox.c
	$(CC) $(CFLAGS) -o raycaster main.c Color.c Sphere.c Vec3.c Ray.c Raycast.c Texture.c Quaternion.c BoundingBox.c