#include "../include/num_redefs.h"
#include "raylib.h"

#ifndef GJK_H_
#define GJK_H_

int FurthestPoint(Vector3 *points, u16 count, Vector3 dir);

#define HULL_MAX_POINTS 32
typedef struct {
	Vector3 points[HULL_MAX_POINTS];

	Vector3 origin;

	u8 point_count;

} Hull;

Vector3 Support(Hull *hull_A , Hull *hull_B, Vector3 dir);
Vector3 HullOrigin(Hull *hull);

#define SIMPLEX_MAX_POINTS 32
typedef struct {
	Vector3 points[32];
	u8 size;

} Simplex;

short DoSimplex(Simplex *simplex, Vector3 *dir);

#endif
