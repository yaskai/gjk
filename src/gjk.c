#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"

Vector3 TripleProduct(Vector3 a, Vector3 b, Vector3 c) {
	return(Vector3CrossProduct(Vector3CrossProduct(a, b), c));
}

int FurthestPoint(Vector3 *points, u16 count, Vector3 dir) {
	float max_dot = -FLT_MAX;
	int id = 0;

	for(u16 i = 0; i < count; i++) {
		float dot = Vector3DotProduct(dir, points[i]);

		if(dot > max_dot) {
			max_dot = dot;
			id = i;
		}
	}

	return id;
}

Vector3 Support(Hull *hull_A , Hull *hull_B, Vector3 dir) {
	int max = FurthestPoint(hull_A->points, hull_A->point_count, dir);
	int min = FurthestPoint(hull_B->points, hull_B->point_count, Vector3Negate(dir));

	return Vector3Subtract(
		hull_A->points[max], hull_B->points[min]
	);
}

// Rough estimate of hull center
Vector3 HullCenter(Hull *hull) {
	float3 min = Vector3ToFloatV(Vector3Scale(Vector3One(),  FLT_MAX));
	float3 max = Vector3ToFloatV(Vector3Scale(Vector3One(), -FLT_MAX));
	
	for(u16 i = 0; i < hull->point_count; i++) {
		float3 point = Vector3ToFloatV(hull->points[i]);

		for(short a = 0; a < 3; a++) {
			if(point.v[a] < min.v[a])	
				min.v[a] = point.v[a];

			if(point.v[a] > max.v[a])	
				max.v[a] = point.v[a];
		}
	}

	float3 out = {0};
	for(short a = 0; a < 3; a++)
		out.v[a] = max.v[a] = min.v[a] * 0.5f;

	return (Vector3) {
		.x = out.v[0],
		.y = out.v[1],
		.z = out.v[2]
	};
}

short DoSimplex(Simplex *simplex, Vector3 *dir) {
	return 0;
}

void GjkIntersect(Hull *hull_A, Hull *hull_B, GjkData *data) {

}

