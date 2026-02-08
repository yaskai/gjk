#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"
#include "box.h"

Vector2 Vec3Flat(Vector3 vec3) { return (Vector2) { .x = vec3.x, .y = vec3.y }; } 
Vector3 vec2Deep(Vector2 vec2) { return (Vector3) { .x = vec2.x, .y = vec2.y, .z = 0 }; }

int main() {
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	InitWindow(1920, 1080, "Raylib Project");
	SetTargetFPS(100);

	Vector3 player_pos = (Vector3) { 0, 0, 0 };

	i8 num_points = 8;
	BoundingBox box_base = (BoundingBox) { .min = Vector3Scale(Vector3One(), -30), .max = Vector3Scale(Vector3One(), 30) };

	BoundingBox box_A = BoxTranslate(box_base, (Vector3) { 0, 0, 0 });
	Hull hull_A = (Hull) {0};
	BoxPoints points_A = BoxGetPoints(box_A);
	memcpy(hull_A.points, points_A.v, sizeof(Vector3) * num_points);
	hull_A.point_count = num_points;
	hull_A.origin = HullCenter(&hull_A);
		
	BoundingBox box_B = BoxTranslate(box_base, (Vector3) { 0, 10, 1 });
	box_B.min.x += 10;
	box_B.max.z -= 10;
	box_B.min.y += 10;
	box_B.max.y -= 10;

	Hull hull_B = (Hull) {0};
	BoxPoints points_B = BoxGetPoints(box_B);
	memcpy(hull_B.points, points_B.v, sizeof(Vector3) * num_points);
	hull_B.point_count = num_points;
	hull_B.origin = HullCenter(&hull_B);

	Camera3D camera = (Camera3D) {
		.position = (Vector3) { 0, 60, 100 },
		.target = (Vector3) { 0, 0, 0 },
		.up = (Vector3) {0, 1, 0},
		.fovy = 60,
		.projection = CAMERA_PERSPECTIVE
	};
	
	while(!WindowShouldClose()) {
		float delta_time = GetFrameTime();

		Vector3 move = Vector3Zero();
		if(IsKeyDown(KEY_A)) move.x--;
		if(IsKeyDown(KEY_D)) move.x++;
		if(IsKeyDown(KEY_W)) move.z--;
		if(IsKeyDown(KEY_S)) move.z++;
		move = Vector3Scale(move, 500 * delta_time);
		player_pos = Vector3Add(player_pos, move);

		for(i8 i = 0; i < hull_B.point_count; i++) {
			hull_B.points[i] = Vector3Add(hull_B.points[i], move);
			hull_B.origin = HullCenter(&hull_B);
			box_B = BoxTranslate(box_B, player_pos);
		}

		GjkData gjk_data = GjkDataEmpty();
		GjkIntersect(&hull_A, &hull_B, &gjk_data);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		DrawBoundingBox(box_A, RED);
		DrawBoundingBox(box_B, GREEN);

		EndMode3D();

		char *text = (gjk_data.hit) ? "collision" : "no collision";
		DrawText(text, 0, 0, 32, RAYWHITE);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void BoxDraw(Vector2 pos, Vector2 extent, Color color) {
	Vector2 h = Vector2Scale(extent, 0.5f);	
	DrawRectangle(pos.x-h.x, pos.y-h.y, h.x*2, h.y*2, color);	
}

