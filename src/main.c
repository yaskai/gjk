#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"
#include "gjk.h"
#include "box.h"

Vector2 Vec3Flat(Vector3 vec3) { return (Vector2) { .x = vec3.x, .y = vec3.y }; } 
Vector3 vec2Deep(Vector2 vec2) { return (Vector3) { .x = vec2.x, .y = vec2.y, .z = 0 }; }

#define CAM_UP (Vector3) { 0, 1, 0 }
#define CAM_UP				(Vector3) { 0, 1, 0 }
#define CAM_MAX_PITCH		(89.0f * DEG2RAD) 
#define CAM_SPEED			100.0f
#define CAM_SENSITIVITY		0.275f
void CameraControls(Camera3D *cam, float dt);
void VirtCameraControls(Camera3D *cam, float dt);
float cam_p, cam_y, cam_r;

int main() {
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	InitWindow(1920, 1080, "Raylib Project");
	SetTargetFPS(100);
	DisableCursor();

	Vector3 player_pos = (Vector3) { -50, 10, 0 };

	float s = 20;
	BoundingBox box_base = (BoundingBox) { .min = Vector3Scale(Vector3One(), -s), .max = Vector3Scale(Vector3One(), s) };

	Model model = LoadModel("shape.glb");
	model.transform = MatrixMultiply(model.transform, MatrixScale(30, 30, 30));

	BoundingBox box_A = BoxTranslate(box_base, (Vector3) { 0, 0, 0 });
	Hull hull_A = (Hull) {0};
	i8 num_points = model.meshes[0].vertexCount;
	printf("%d\n", model.meshCount);

	Vector3 *points_A = malloc(sizeof(Vector3) * num_points);
	Mesh mesh = model.meshes[0];
	Vector3 *verts = (Vector3 *)mesh.vertices;
	for(int i = 0; i < mesh.vertexCount; i++) {
		points_A[i] = Vector3Transform(verts[i], MatrixScale(30, 30, 30));
	}
	free(verts);

	memcpy(hull_A.points, points_A, sizeof(Vector3) * num_points);

	hull_A.point_count = num_points;
	hull_A.origin = HullCenter(&hull_A);
		
	num_points = 8;
	BoundingBox box_B = BoxTranslate(box_base, (Vector3) { 0, 0, 0 });
	Hull hull_B = (Hull) {0};
	BoxPoints points_B = BoxGetPoints(box_B);
	memcpy(hull_B.points, points_B.v, sizeof(Vector3) * num_points);
	for(short i = 0; i < num_points; i++) {
		hull_B.points[i] = Vector3Add(hull_B.points[i], player_pos);
	}
	hull_B.point_count = num_points;
	hull_B.origin = HullCenter(&hull_B);

	Camera3D camera = (Camera3D) {
		.position = (Vector3) { 0, 60, 100 },
		.target = (Vector3) { 0, 0, 0 },
		.up = (Vector3) {0, 1, 0},
		.fovy = 90,
		.projection = CAMERA_PERSPECTIVE
	};
	
	while(!WindowShouldClose()) {
		float delta_time = GetFrameTime();

		CameraControls(&camera, delta_time);
		VirtCameraControls(&camera, delta_time);

		Vector3 move = Vector3Zero();
		if(IsKeyDown(KEY_H)) move.x--;
		if(IsKeyDown(KEY_L)) move.x++;
		if(IsKeyDown(KEY_K)) move.z--;
		if(IsKeyDown(KEY_J)) move.z++;
		move = Vector3Scale(move, 100 * delta_time);
		Vector3 wish_point = Vector3Add(player_pos, move);

		GjkData gjk_data = GjkDataEmpty();
		GjkIntersect(&hull_A, &hull_B, &gjk_data);

		/*
		if(Vector3Length(move) > 0) {
			Hull collision_hull = (Hull) {0};
			BoundingBox coll_box = BoxTranslate(box_B, wish_point);
			BoxPoints coll_points = BoxGetPoints(coll_box);
			memcpy(collision_hull.points, coll_points.v, sizeof(Vector3) * 8);
			collision_hull.point_count = 8;
			collision_hull.origin = HullCenter(&collision_hull);
			GjkIntersect(&hull_A, &collision_hull, &gjk_data);
		}

		if(!gjk_data.hit) {
			player_pos = wish_point;
			for(i8 i = 0; i < hull_B.point_count; i++) {
				hull_B.points[i] = Vector3Add(hull_B.points[i], move);
				hull_B.origin = HullCenter(&hull_B);
			}
		}
		else wish_point = player_pos;
		*/
		for(i8 i = 0; i < hull_B.point_count; i++) {
			hull_B.points[i] = Vector3Add(hull_B.points[i], move);
			hull_B.origin = HullCenter(&hull_B);
		}
		player_pos = wish_point;
		box_B = BoxTranslate(box_B, player_pos);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		DrawModel(model, Vector3Zero(), 1, PURPLE);
		DrawModelWires(model, Vector3Zero(), 1, DARKBLUE);

		for(i8 i = 0; i < hull_A.point_count; i++) {
			DrawSphere(hull_A.points[i], 1.5f, RED);
		}

		for(i8 i = 0; i < hull_B.point_count; i++) {
			DrawSphere(hull_B.points[i], 1.5f, RED);
		}

		//DrawBoundingBox(box_A, RED);
		DrawBoundingBox(box_B, GREEN);
		//DrawBoundingBox(coll_box, SKYBLUE);

		EndMode3D();

		if(gjk_data.hit == 1)
			DrawText("hit", 0, 0, 32, RAYWHITE);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}

void BoxDraw(Vector2 pos, Vector2 extent, Color color) {
	Vector2 h = Vector2Scale(extent, 0.5f);	
	DrawRectangle(pos.x-h.x, pos.y-h.y, h.x*2, h.y*2, color);	
}

void CameraControls(Camera3D *cam, float dt) {
	Vector2 mouse_delta = GetMouseDelta();

	cam_p -= mouse_delta.y * CAM_SENSITIVITY * dt;
	cam_y += mouse_delta.x * CAM_SENSITIVITY * dt;
	cam_p = Clamp(cam_p, -CAM_MAX_PITCH, CAM_MAX_PITCH);	

	Vector3 forward = Vector3Normalize((Vector3) { .x = cosf(cam_y) * cosf(cam_p), .y = sinf(cam_p), .z = sinf(cam_y) * cosf(cam_p) }); 
	Vector3 right = Vector3CrossProduct(forward, CAM_UP);
	
	cam->target = Vector3Add(cam->position, forward); 

	Vector3 movement = Vector3Zero();	

	if(IsKeyDown(KEY_W)) movement = Vector3Add(movement, forward);
	if(IsKeyDown(KEY_D)) movement = Vector3Add(movement, right);
	if(IsKeyDown(KEY_S)) movement = Vector3Subtract(movement, forward);
	if(IsKeyDown(KEY_A)) movement = Vector3Subtract(movement, right);

	movement = Vector3Scale(movement, CAM_SPEED * dt);

	cam->position = Vector3Add(cam->position, movement);
	cam->target = Vector3Add(cam->target, movement);
}

void VirtCameraControls(Camera3D *cam, float dt) {
	Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position)); 
	Vector3 right = Vector3CrossProduct(forward, CAM_UP);
	
	Vector3 movement = Vector3Zero();	

	movement = Vector3Add(movement, Vector3Scale(forward, GetMouseWheelMove()));

	if(IsKeyDown(KEY_UP)) 		movement = Vector3Add(movement, CAM_UP);
	if(IsKeyDown(KEY_RIGHT)) 	movement = Vector3Add(movement, right);
	if(IsKeyDown(KEY_DOWN))		movement = Vector3Subtract(movement, CAM_UP);
	if(IsKeyDown(KEY_LEFT))		movement = Vector3Subtract(movement, right);

	movement = Vector3Scale(movement, CAM_SPEED * dt);
	
	cam->position = Vector3Add(cam->position, movement);
	cam->target = Vector3Add(cam->target, movement);
}

