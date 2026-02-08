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
 
	Hull hull_A = (Hull) {0};
	Vector3 points_A[] = {
		{   500,  300,   0 },
		{  1000,  900,   0 },
		{  1350,  500,   0 },
		{  400,   600,   0 },
		{  550,   900,   0 },
		{  990,   200,   0 }
	};
	size_t numpoints = (sizeof(points_A) / sizeof(points_A[0]));
	hull_A.point_count = numpoints;
	memcpy(hull_A.points, points_A, sizeof(Vector3) * numpoints);
	hull_A.origin = HullOrigin(&hull_A);

	Hull hull_B = (Hull) {0};
	Vector3 points_B[] = {
		{  200, 150,  0 },
		{  332,  80,  0 },
		{   64,  32,  0 },
	};
	numpoints = (sizeof(points_B) / sizeof(points_B[0]));
	hull_B.point_count = numpoints;
	memcpy(hull_B.points, points_B, sizeof(Vector3) * numpoints);
	hull_B.origin = HullOrigin(&hull_B);

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

		for(u16 i = 0; i < hull_A.point_count; i++) {
			DrawCircleV(Vec3Flat(hull_A.points[i]), 8, RAYWHITE);
		}

		for(u16 i = 0; i < hull_B.point_count; i++) {
			hull_B.points[i] = Vector3Add(hull_B.points[i], vec2Deep(move));
			hull_B.origin = HullOrigin(&hull_B);
			DrawCircleV(Vec3Flat(hull_B.points[i]), 8, PINK);
		}

		Vector3 s = Support(&hull_A, &hull_B, (Vector3) {1, 0, 0} );
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

