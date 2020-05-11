SHELL=/bin/bash
CC=gcc
CFLAGS=-std=c11 -Wall -W -ggdb3 -O0 -lm

raycaster: main.c Color.c Sphere.c Vec3.c Ray.c Raycast.c Consts.h
	$(CC) $(CFLAGS) -o raycaster main.c Color.c Sphere.c Vec3.c Ray.c Raycast.c