#include <stdio.h>
#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"

Vector3 TripleProduct(Vector3 a, Vector3 b, Vector3 c) {
	//return(Vector3CrossProduct(Vector3CrossProduct(a, b), c));

	float ac = Vector3DotProduct(a, c);
	float bc = Vector3DotProduct(b, c);

	return Vector3Subtract(
		Vector3Scale(b, ac),
		Vector3Scale(a, bc)
	);
}

short FurthestPoint(Vector3 *points, u16 count, Vector3 dir) {
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
		out.v[a] = (min.v[a] + max.v[a]) * 0.5f;

	return (Vector3) {
		.x = out.v[0],
		.y = out.v[1],
		.z = out.v[2]
	};
}

void PushToSimplex(Simplex *simplex, Vector3 point) {
	/*
	if(simplex->size < SIMPLEX_TETRA) {
		simplex->points[simplex->size++] = point;
	}
	*/

	for(short i = simplex->size; i > 0; i--) {
		simplex->points[i] = simplex->points[i-1];
	}
	simplex->points[0] = point;
	simplex->size++;
}

short DoSimplex(Simplex *simplex, Vector3 *dir) {
	switch(simplex->size) {
		case SIMPLEX_POINT:
			*dir = Vector3Negate(simplex->points[0]);
			return 0;
		case SIMPLEX_LINE:
			return SimplexLine(simplex, dir);
		case SIMPLEX_TRI:
			return SimplexTri(simplex, dir);
		case SIMPLEX_TETRA:
			return SimplexTetra(simplex, dir);
		default:
			return 0;
	}
}

short SimplexLine(Simplex *simplex, Vector3 *dir) {
	// A is always most recent point
	/*
	Vector3 a = simplex->points[1];	
	Vector3 b = simplex->points[0];	
	*/
	Vector3 a = simplex->points[0];	
	Vector3 b = simplex->points[1];	

	Vector3 ab = Vector3Subtract(b, a); // A -> B
	Vector3 ao = Vector3Negate(a); 		// A -> Origin

	if(Vector3DotProduct(ab, ao) > 0) {
		// Origin is in front of AB
		// New direction is perpendicular to AB towards origin
		*dir = TripleProduct(ab, ao, ab);
	} else {
		// Origin behind A, keep A only
		simplex->points[0] = a;
		simplex->size = SIMPLEX_POINT;
		*dir = ao;
	}

	if(Vector3LengthSqr(*dir) < 1e-8f) *dir = Vector3CrossProduct(ab, Vector3CrossProduct(ab, ao));

	//*dir = Vector3Normalize(*dir);

	// Line can't contain origin in 3D
	return 0;
}

short SimplexTri(Simplex *simplex, Vector3 *dir) {
	/*
	Vector3 a = simplex->points[2];
	Vector3 b = simplex->points[1];
	Vector3 c = simplex->points[0];
	*/

	Vector3 a = simplex->points[0];	
	Vector3 b = simplex->points[1];	
	Vector3 c = simplex->points[2];	

	Vector3 ab = Vector3Subtract(b, a);
	Vector3 ac = Vector3Subtract(c, a);
	Vector3 bc = Vector3Subtract(c, b);

	Vector3 ao = Vector3Negate(a);
	//Vector3 bo = Vector3Negate(b);

	// Triangle normal
	Vector3 abc = Vector3CrossProduct(ab, ac);
	
	// Edge AB
	Vector3 ab_perp = Vector3CrossProduct(ab, abc);
	if(Vector3DotProduct(ab_perp, ao) > 0) {
		// Origin outside AB edge
		/*
		simplex->points[0] = b;
		simplex->points[1] = a;
		*/
		simplex->points[0] = a;
		simplex->points[1] = b;
		simplex->size = SIMPLEX_LINE;
		*dir = TripleProduct(ab, ao, ab);
		if(Vector3LengthSqr(*dir) < 1e-8f) *dir = Vector3CrossProduct(ab, Vector3CrossProduct(ab, ao));
		return 0;
	}

	// Edge AC
	Vector3 ac_perp = Vector3CrossProduct(abc, ac);
	if(Vector3DotProduct(ac_perp, ao) > 0) {
		// Origin outside AC edge
		simplex->points[0] = a;
		simplex->points[1] = c;
		simplex->size = SIMPLEX_LINE;
		*dir = TripleProduct(ac, ao, ac);
		//*dir = Vector3Normalize(*dir);
		if(Vector3LengthSqr(*dir) < 1e-8f) *dir = Vector3CrossProduct(ac, Vector3CrossProduct(ab, ao));
		return 0;
	}
	
	// Edge BC
	Vector3 bo = Vector3Negate(b);
	Vector3 bc_perp = Vector3CrossProduct(bc, abc);
	if(Vector3DotProduct(bc_perp, bo) > 0) {
		// Origin outside AC edge
		simplex->points[0] = c;
		simplex->points[1] = b;
		simplex->size = SIMPLEX_LINE;
		*dir = TripleProduct(bc, bo, bc);
		//dir = Vector3Normalize(*dir);
		if(Vector3LengthSqr(*dir) < 1e-8f) *dir = Vector3CrossProduct(bc, Vector3CrossProduct(bc, bo));
		return 0;
	}

	// Check if tri is above or below
	if(Vector3DotProduct(abc, ao) > 0) {
		// Above, keep the same tri
		*dir = abc;
	} else {
		// Below, flip winding order
		/*
		simplex->points[1] = c;
		simplex->points[2] = b;	
		*dir = Vector3Negate(abc);
		*/
		Vector3 temp = simplex->points[1];
		simplex->points[1] = simplex->points[2];
		simplex->points[2] = temp;
		*dir = Vector3Negate(abc);
	}

	if(Vector3LengthSqr(*dir) < 1e-8f) *dir = Vector3CrossProduct(ab, Vector3CrossProduct(ab, ao));

	return 0;
}

short SimplexTetra(Simplex *simplex, Vector3 *dir) {
	/*
	Vector3 a = simplex->points[3];
	Vector3 b = simplex->points[2];
	Vector3 c = simplex->points[1];
	Vector3 d = simplex->points[0];
	*/

	Vector3 a = simplex->points[0];	
	Vector3 b = simplex->points[1];	
	Vector3 c = simplex->points[2];	
	Vector3 d = simplex->points[3];

	// Check each face of tetrahedron
	if(TetraCheckFace(a, b, c, d, simplex, dir)) return 0;

	if(TetraCheckFace(a, b, d, c, simplex, dir)) return 0;

	if(TetraCheckFace(a, c, d, b, simplex, dir)) return 0;

	if(TetraCheckFace(b, c, d, a, simplex, dir)) return 0;
	
	// Return collision
	return 1;
}

short TetraCheckFace(Vector3 a, Vector3 b, Vector3 c, Vector3 op, Simplex *simplex, Vector3 *dir) {
	Vector3 ab = Vector3Subtract(b, a);	
	Vector3 ac = Vector3Subtract(c, a);
	Vector3 norm = Vector3CrossProduct(ab, ac);

	float opp_side = Vector3DotProduct(norm, Vector3Subtract(op, a));
	if(opp_side > 0) norm = Vector3Negate(norm);			

	float origin_side = Vector3DotProduct(norm, Vector3Negate(a));
	if(origin_side > 0) {
		return 0;
	}

	if(opp_side * origin_side < 0) {
		/*
		simplex->points[0] = c; 
		simplex->points[1] = b; 
		simplex->points[2] = a; 
		*/
		simplex->points[0] = a; 
		simplex->points[1] = b; 
		simplex->points[2] = c; 
		simplex->size = SIMPLEX_TRI;
		*dir = norm;
		//*dir = Vector3Normalize(*dir);
		return 1;
	}

	return 0;
}

GjkData GjkDataEmpty() {
	return (GjkData) {
		.point = Vector3Zero(),
		.distance = FLT_MAX,
		.iterations = 0,
		.hit = 0,
	};
}

void GjkIntersect(Hull *hull_A, Hull *hull_B, GjkData *data) {
	Vector3 center_A = HullCenter(hull_A);
	Vector3 center_B = HullCenter(hull_B);

	Vector3 dir = Vector3Subtract(center_B, center_A);

	if(Vector3LengthSqr(dir) <= EPSILON) 
		dir = (Vector3) { 1, 0, 0 };

	Simplex simplex = (Simplex) {0};

	// Add first support point
	Vector3 first_point = Support(hull_A, hull_B, dir);
	PushToSimplex(&simplex, first_point);

	// Direction towards origin
	dir = Vector3Negate(first_point);
	//if(Vector3Length(dir) > EPSILON) dir = Vector3Normalize(dir);

	for(short i = 0; i < GJK_MAX_ITERS; i++) {
		data->iterations = i;

		Vector3 new_point = Support(hull_A, hull_B, dir);

		if(Vector3DotProduct(new_point, dir) <= 1e-6f) {
			data->hit = 0;
			return;
		}

		PushToSimplex(&simplex, new_point);

		if(DoSimplex(&simplex, &dir)) {
			data->hit = 1;
			return;
		}
	}

	data->hit = 0;
}
