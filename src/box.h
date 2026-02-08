#include "raylib.h"

#ifndef BOX_H_
#define BOX_H_

Vector3 BoxExtent(BoundingBox box);

// Find center point of box
Vector3 BoxCenter(BoundingBox box);

// Return a box with min as [float max] and max as [float min]
BoundingBox EmptyBox();

// Grow a box to fit a point in space
BoundingBox BoxExpandToPoint(BoundingBox box, Vector3 point);

// Translate a box to a new position
BoundingBox BoxTranslate(BoundingBox box, Vector3 point);

typedef struct {
	Vector3 v[8];
	
} BoxPoints; 

BoxPoints BoxGetPoints(BoundingBox box);

#endif
