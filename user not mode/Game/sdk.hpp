#pragma once
#include <d3d9.h>
#include <vector>
#include <numbers>
#define M_PI 3.14159265358979323846264338327950288419716939937510

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(double _x, double _y) : x(_x), y(_y) {}
	~Vector2() {}
	double x, y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	~Vector3() {}
	double x, y, z;
	inline double dot(Vector3 v) { return x * v.x + y * v.y + z * v.z; }
	inline double distance(Vector3 v) { return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0))); }
	Vector3 operator-(Vector3 v) { return Vector3(x - v.x, y - v.y, z - v.z); }
};

struct FQuat { double x, y, z, w; };
struct FTransform
{
	FQuat rotation;
	Vector3 translation;
	uint8_t pad1c[0x8];
	Vector3 scale3d;
	uint8_t pad2c[0x8];

	D3DMATRIX to_matrix_with_scale()
	{
		D3DMATRIX m{};

		const Vector3 Scale
		(
			(scale3d.x == 0.0) ? 1.0 : scale3d.x,
			(scale3d.y == 0.0) ? 1.0 : scale3d.y,
			(scale3d.z == 0.0) ? 1.0 : scale3d.z
		);

		const double x2 = rotation.x + rotation.x;
		const double y2 = rotation.y + rotation.y;
		const double z2 = rotation.z + rotation.z;
		const double xx2 = rotation.x * x2;
		const double yy2 = rotation.y * y2;
		const double zz2 = rotation.z * z2;
		const double yz2 = rotation.y * z2;
		const double wx2 = rotation.w * x2;
		const double xy2 = rotation.x * y2;
		const double wz2 = rotation.w * z2;
		const double xz2 = rotation.x * z2;
		const double wy2 = rotation.w * y2;

		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;
		m._11 = (1.0f - (yy2 + zz2)) * Scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * Scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * Scale.z;
		m._32 = (yz2 - wx2) * Scale.z;
		m._23 = (yz2 + wx2) * Scale.y;
		m._21 = (xy2 - wz2) * Scale.y;
		m._12 = (xy2 + wz2) * Scale.x;
		m._31 = (xz2 + wy2) * Scale.z;
		m._13 = (xz2 - wy2) * Scale.x;
		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

inline D3DMATRIX matrix_multiplication(D3DMATRIX PM1, D3DMATRIX PM2) {
	D3DMATRIX POut;
	POut._11 = PM1._11 * PM2._11 + PM1._12 * PM2._21 + PM1._13 * PM2._31 + PM1._14 * PM2._41;
	POut._12 = PM1._11 * PM2._12 + PM1._12 * PM2._22 + PM1._13 * PM2._32 + PM1._14 * PM2._42;
	POut._13 = PM1._11 * PM2._13 + PM1._12 * PM2._23 + PM1._13 * PM2._33 + PM1._14 * PM2._43;
	POut._14 = PM1._11 * PM2._14 + PM1._12 * PM2._24 + PM1._13 * PM2._34 + PM1._14 * PM2._44;
	POut._21 = PM1._21 * PM2._11 + PM1._22 * PM2._21 + PM1._23 * PM2._31 + PM1._24 * PM2._41;
	POut._22 = PM1._21 * PM2._12 + PM1._22 * PM2._22 + PM1._23 * PM2._32 + PM1._24 * PM2._42;
	POut._23 = PM1._21 * PM2._13 + PM1._22 * PM2._23 + PM1._23 * PM2._33 + PM1._24 * PM2._43;
	POut._24 = PM1._21 * PM2._14 + PM1._22 * PM2._24 + PM1._23 * PM2._34 + PM1._24 * PM2._44;
	POut._31 = PM1._31 * PM2._11 + PM1._32 * PM2._21 + PM1._33 * PM2._31 + PM1._34 * PM2._41;
	POut._32 = PM1._31 * PM2._12 + PM1._32 * PM2._22 + PM1._33 * PM2._32 + PM1._34 * PM2._42;
	POut._33 = PM1._31 * PM2._13 + PM1._32 * PM2._23 + PM1._33 * PM2._33 + PM1._34 * PM2._43;
	POut._34 = PM1._31 * PM2._14 + PM1._32 * PM2._24 + PM1._33 * PM2._34 + PM1._34 * PM2._44;
	POut._41 = PM1._41 * PM2._11 + PM1._42 * PM2._21 + PM1._43 * PM2._31 + PM1._44 * PM2._41;
	POut._42 = PM1._41 * PM2._12 + PM1._42 * PM2._22 + PM1._43 * PM2._32 + PM1._44 * PM2._42;
	POut._43 = PM1._41 * PM2._13 + PM1._42 * PM2._23 + PM1._43 * PM2._33 + PM1._44 * PM2._43;
	POut._44 = PM1._41 * PM2._14 + PM1._42 * PM2._24 + PM1._43 * PM2._34 + PM1._44 * PM2._44;

	return POut;
}

D3DMATRIX to_matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);
	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

struct Camera
{
	Vector3 location;
	Vector3 rotation;
	float fov;
};

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

namespace cache
{
	inline uintptr_t uworld;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;

	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline uintptr_t closest_aactor;
	inline Vector3 relative_location;
	inline Vector3 plocaldistance;
	inline int my_team_id;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline int player_count;
	inline float closest_distance;
	inline uintptr_t closest_mesh;
	inline Camera local_camera;
}


Camera get_view_point()
{
	Camera view_point{};
	uintptr_t location_pointer = read<uintptr_t>(cache::uworld + offsets::CameraLocation);
	uintptr_t rotation_pointer = read<uintptr_t>(cache::uworld + offsets::CameraRotation);
	FNRot fnrot{};
	fnrot.a = read<double>(rotation_pointer);
	fnrot.b = read<double>(rotation_pointer + 0x20);
	fnrot.c = read<double>(rotation_pointer + 0x1D0);
	view_point.location = read<Vector3>(location_pointer);
	view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
	view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
	view_point.fov = read<float>(cache::player_controller + offsets::CameraFOV) * 90.0f;
	return view_point;
}

Vector2 project_world_to_screen(Vector3 world_location)
{
	cache::local_camera = get_view_point();
	D3DMATRIX temp_matrix = to_matrix(cache::local_camera.rotation);
	Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - cache::local_camera.location;
	Vector3 vtransformed = Vector3(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
	if (vtransformed.z < 1) vtransformed.z = 1;
	return Vector2(settings::screen_center_x + vtransformed.x * ((settings::screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z, settings::screen_center_y - vtransformed.y * ((settings::screen_center_x / tanf(cache::local_camera.fov * M_PI / 360))) / vtransformed.z);
}

Vector3 get_entity_bone(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = read<uintptr_t>(mesh + offsets::BoneArray);
	if (bone_array == NULL) bone_array = read<uintptr_t>(mesh + offsets::BoneArray + 0x10);
	FTransform bone = read<FTransform>(bone_array + (bone_id * 0x60)); 
	FTransform component_to_world = read<FTransform>(mesh + offsets::ComponentToWorld);
	D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

bool is_visible(uintptr_t Mesh) {
	auto Seconds = read<double>(cache::uworld + offsets::CameraRotation + 0x10);
	auto LastRenderTime = read<float>(Mesh + offsets::LastRenderTime);
	return Seconds - LastRenderTime <= 0.06f;
}

