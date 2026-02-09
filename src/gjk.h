#include "../include/num_redefs.h"
#include "raylib.h"

#ifndef GJK_H_
#define GJK_H_

Vector3 TripleProduct(Vector3 a, Vector3 b, Vector3 c);
short FurthestPoint(Vector3 *points, u16 count, Vector3 dir);

#define HULL_MAX_POINTS 32
typedef struct {
	Vector3 points[HULL_MAX_POINTS];

	Vector3 origin;

	u8 point_count;

} Hull;

Vector3 Support(Hull *hull_A , Hull *hull_B, Vector3 dir);
Vector3 HullCenter(Hull *hull);

#define SIMPLEX_EMPTY 	0
#define SIMPLEX_POINT   1
#define SIMPLEX_LINE 	2
#define SIMPLEX_TRI 	3
#define SIMPLEX_TETRA	4

#define SIMPLEX_MAX_POINTS 4

typedef struct {
	Vector3 points[SIMPLEX_MAX_POINTS];

	// Size refers to shape of simplex
	// point, line, triangle, tetrahedron 
	u8 size;

} Simplex;

void PushToSimplex(Simplex *simplex, Vector3 point);
short DoSimplex(Simplex *simplex, Vector3 *dir);

short SimplexLine(Simplex *simplex, Vector3 *dir);
short SimplexTri(Simplex *simplex, Vector3 *dir);
short SimplexTetra(Simplex *simplex, Vector3 *dir);

short TetraCheckFace(Vector3 a,
					 Vector3 b,
					 Vector3 c,
					 Vector3 op,
					 Simplex *simplex,
					 Vector3 *dir
					 ); 

#define GJK_MAX_ITERS 128
typedef struct {
	Vector3 point;
	float distance;

	short iterations;	

	short hit;

} GjkData;

GjkData GjkDataEmpty();
void GjkIntersect(Hull *hull_A, Hull *hull_B, GjkData *data);

#endif
