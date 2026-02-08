#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"

typedef struct {
	Vector2 point_A;
	Vector2 point_B;

	Vector2 normal;
	Vector2 center;

} Line;

void BoxDraw(Vector2 pos, Vector2 extent, Color color);

Vector2 Vec3Flat(Vector3 vec3) { return (Vector2) { .x = vec3.x, .y = vec3.y }; } 
Vector3 vec2Deep(Vector2 vec2) { return (Vector3) { .x = vec2.x, .y = vec2.y, .z = 0 }; }

int main() {
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	InitWindow(1920, 1080, "Raylib Project");
	SetTargetFPS(100);

	Vector2 player_pos = (Vector2) { 700, 300 };
	Vector2 player_ext = (Vector2) { 100, 100 };
 
	Hull hull = (Hull) {0};
	Vector3 points[] = {
		{   100,  300,   0 },
		{  1400,  1000,  0 },
		{  1000,  500,   0 }
	};

	size_t numpoints = (sizeof(points) / sizeof(points[0]));
	hull.point_count = numpoints;
	memcpy(hull.points, points, sizeof(Vector3) * numpoints);

	Vector2 cursor_dir = {0};
	
	while(!WindowShouldClose()) {
		float delta_time = GetFrameTime();

		Vector2 mouse_pos = GetMousePosition();
		cursor_dir = Vector2Subtract(mouse_pos, player_pos);

		Vector2 move = Vector2Zero();
		if(IsKeyDown(KEY_A)) move.x--;
		if(IsKeyDown(KEY_D)) move.x++;
		if(IsKeyDown(KEY_W)) move.y--;
		if(IsKeyDown(KEY_S)) move.y++;
		move = Vector2Scale(move, 500 * delta_time);
		player_pos = Vector2Add(player_pos, move);

		BeginDrawing();
		ClearBackground(BLACK);

		for(u16 i = 0; i < hull.point_count; i++) {
			DrawCircleV(Vec3Flat(hull.points[i]), 8, RAYWHITE);
		}

		DrawCircleV(player_pos, 32, GREEN);

		DrawLineV(player_pos, Vector2Scale(Vector2Normalize(cursor_dir), 2000), GREEN);
		Vector3 cursor_end = vec2Deep(Vector2Scale(cursor_dir, 2000));

		int furthest = FurthestPoint(hull.points, hull.point_count, vec2Deep(cursor_dir));
		if(furthest != -1) 
			DrawCircleV(Vec3Flat(hull.points[furthest]), 8, RED);

		Vector3 s = Support(hull.points, hull.point_count, vec2Deep(cursor_dir));
		DrawCircleV(Vec3Flat(s), 8, SKYBLUE);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void BoxDraw(Vector2 pos, Vector2 extent, Color color) {
	Vector2 h = Vector2Scale(extent, 0.5f);	
	DrawRectangle(pos.x-h.x, pos.y-h.y, h.x*2, h.y*2, color);	
}

