SHELL=/bin/bash
CC=gcc
CFLAGS=-std=c11 -Wall -W -ggdb3 -O3 -lm -lpthread

raycaster: main.c Color.c geometry/Sphere.c math/Vec3.c geometry/Ray.c Raycast.c Consts.h Texture.c math/Quaternion.c geometry/BoundingBox.c math/Transform.c math/Matrix4.c geometry/TriangleMesh.c geometry/Triangle.c geometry/SurfaceHit.c ObjectWrapper.c
	$(CC) $(CFLAGS) -o raycaster main.c Color.c geometry/Sphere.c math/Vec3.c geometry/Ray.c Raycast.c Texture.c math/Quaternion.c geometry/BoundingBox.c math/Transform.c math/Matrix4.c geometry/TriangleMesh.c geometry/Triangle.c geometry/SurfaceHit.c ObjectWrapper.c