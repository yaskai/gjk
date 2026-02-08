#include <float.h>
#include "raylib.h"
#include "raymath.h"
#include "box.h"
#include "../include/num_redefs.h"

// Calculate length of a box in each axis
Vector3 BoxExtent(BoundingBox box) {
	return Vector3Subtract(box.max, box.min);
}

// Compute surface area of box
float BoxSurfaceArea(BoundingBox box) {
	Vector3 extent = BoxExtent(box); 
	return (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
}

// Find center point of box
Vector3 BoxCenter(BoundingBox box) {
	Vector3 extent = BoxExtent(box);
	return Vector3Subtract(box.max, Vector3Scale(extent, 0.5f));
}

// Return a box with min as [float max] and max as [float min]
BoundingBox EmptyBox() {
	return (BoundingBox) { .min = Vector3Scale(Vector3One(), FLT_MAX), .max = Vector3Scale(Vector3One(), -FLT_MAX) };
}

// Grow a box to fit a point in space
BoundingBox BoxExpandToPoint(BoundingBox box, Vector3 point) {
	return (BoundingBox) { .min = Vector3Min(box.min, point), .max = Vector3Max(box.max, point) };
}

// Translate a box to a new position
BoundingBox BoxTranslate(BoundingBox box, Vector3 point) {
	Vector3 extent = BoxExtent(box);	
	return (BoundingBox) { .min = Vector3Add(point, Vector3Scale(extent, -0.5f)), .max = Vector3Add(point, Vector3Scale(extent, 0.5f)) };
}

BoxPoints BoxGetPoints(BoundingBox box) {
	BoxPoints box_points = (BoxPoints) {0};

	for(u8 i = 0; i < 8; i++) {
		box_points.v[i].x = (i & 0x01) ? box.min.x : box.max.x;
		box_points.v[i].y = (i & 0x02) ? box.min.y : box.max.y;
		box_points.v[i].z = (i & 0x04) ? box.min.z : box.max.z;
	}

	return box_points;
}

