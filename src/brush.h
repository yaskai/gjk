#include "raylib.h"
#include "../include/num_redefs.h"

#ifndef BRUSH_H_
#define BRUSH_H_

typedef struct {
	Vector3 normal;
	float dist;

} Plane;

typedef struct {
	BoundingBox bounds;

	Vector3 verts[18];
	Plane planes[16];

	i8 vert_count;
	i8 plane_count;

} Brush;

#endif

