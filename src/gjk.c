#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"

int FurthestPoint(Vector3 *points, u16 count, Vector3 dir) {
	float max_dot = -FLT_MAX;
	int id = -1;

	for(u16 i = 0; i < count; i++) {
		float dot = Vector3DotProduct(dir, points[i]);

		if(dot <= 0)
			continue;

		if(dot > max_dot) {
			max_dot = dot;
			id = i;
		}
	}

	return id;
}

Vector3 Support(Vector3 *points, u16 count, Vector3 dir) {
	int min = FurthestPoint(points, count, Vector3Negate(dir));
	if(min < 0) min = 0;

	int max = FurthestPoint(points, count, dir);
	if(max < 0) max = 0;

	return Vector3Subtract(
		points[max], points[min]
	);
}

void DoSimplex(Simplex *simplex, Vector3 dir) {

}

