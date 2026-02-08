#include "../include/num_redefs.h"
#include "raylib.h"

#ifndef GJK_H_
#define GJK_H_

Vector3 TripleProduct(Vector3 a, Vector3 b, Vector3 c);

int FurthestPoint(Vector3 *points, u16 count, Vector3 dir);

#define HULL_MAX_POINTS 32
typedef struct {
	Vector3 points[HULL_MAX_POINTS];

	Vector3 origin;

	u8 point_count;

} Hull;

Vector3 Support(Hull *hull_A , Hull *hull_B, Vector3 dir);
Vector3 HullCenter(Hull *hull);

#define SIMPLEX_MAX_POINTS 32
#define SIMPLEX_POINT 	0
#define SIMPLEX_LINE 	1
#define SIMPLEX_TRI 	2
#define SIMPLEX_TETRA	3
typedef struct {
	Vector3 points[32];

	// Size refers to shape of simplex
	// point, line, triangle, tetrahedron 
	u8 size;

} Simplex;

short DoSimplex(Simplex *simplex, Vector3 *dir);

typedef struct {
	Vector3 point;
	float distance;

	short hit;

} GjkData;

void GjkIntersect(Hull *hull_A, Hull *hull_B, GjkData *data);

#endif
