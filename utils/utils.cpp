#include "utils.hpp"
#include "globals.hpp"
#include "memory/memory.hpp"
#include "../features/aimbot.hpp"
#include "../features/features.hpp"

bool IsValidPointer(uint64_t address) {
	bool res = false;

	(address > 0x10000) && (address < 0x7FFFFFFEFFFF) ? res = true : res = false;

	return res;
}

// Implementation for MatrixTranspose
Matrix* MatrixTranspose(Matrix* pout, const Matrix* pm)
{
	const Matrix m = *pm;
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) pout->m[i][j] = m.m[j][i];

	return pout;
}


// IsInCircle implementation
bool IsInCircle(Vector2 circle_pos, int rad, Vector2 point)
{
	// Compare radius of circle with distance  
	// of its center from given point 
	if ((point.x - circle_pos.x) * (point.x - circle_pos.x) +
		(point.y - circle_pos.y) * (point.y - circle_pos.y) <= rad * rad)
		return true;
	else
		return false;
}

// Implementation for Vec3Dot
float Vec3Dot(const Vector3* pV1, const Vector3* pV2)
{

	return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Vector4 QuaternionLookRotation(Vector3 forward, Vector3 up)
{
	Vector3 vector = forward.Normalized();
	Vector3 vector2; CrossProduct(up, vector).Normalized();
	Vector3 vector3; CrossProduct(vector, vector2);

	auto m00 = vector2.x;
	auto m01 = vector2.y;
	auto m02 = vector2.z;
	auto m10 = vector3.x;
	auto m11 = vector3.y;
	auto m12 = vector3.z;
	auto m20 = vector.x;
	auto m21 = vector.y;
	auto m22 = vector.z;

	float num8 = (m00 + m11) + m22;
	Vector4 quaternion;
	if (num8 > 0.f)
	{
		auto num = (float)sqrt(num8 + 1.f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m12 - m21) * num;
		quaternion.y = (m20 - m02) * num;
		quaternion.z = (m01 - m10) * num;
		return quaternion;
	}
	if ((m00 >= m11) && (m00 >= m22))
	{
		auto num7 = (float)sqrt(((1.f + m00) - m11) - m22);
		auto num4 = 0.5f / num7;
		quaternion.x = 0.5f * num7;
		quaternion.y = (m01 + m10) * num4;
		quaternion.z = (m02 + m20) * num4;
		quaternion.w = (m12 - m21) * num4;
		return quaternion;
	}
	if (m11 > m22)
	{
		auto num6 = (float)sqrt(((1.f + m11) - m00) - m22);
		auto num3 = 0.5f / num6;
		quaternion.x = (m10 + m01) * num3;
		quaternion.y = 0.5f * num6;
		quaternion.z = (m21 + m12) * num3;
		quaternion.w = (m20 - m02) * num3;
		return quaternion;
	}
	auto num5 = (float)sqrt(((1.f + m22) - m00) - m11);
	auto num2 = 0.5f / num5;
	quaternion.x = (m20 + m02) * num2;
	quaternion.y = (m21 + m12) * num2;
	quaternion.z = 0.5f * num5;
	quaternion.w = (m01 - m10) * num2;
	return quaternion;
}

// Implementation for Math::Vector2's constructor
Vector2::Vector2()
{
	// Initialize x, y to 0, 0
	x = 0, y = 0;
}

// Implementation for Math::Vector2's constructor
Vector2::Vector2(float init_x, float init_y)
{
	// Initialize x, y to user-defined values
	x = init_x, y = init_y;
}

// Implementation for Math::Vector2::Lerp (end, t)
Vector2 Vector2::Lerp(Vector2 end, float t)
{
	// Clamp 't' between 0 and 1
	t = std::clamp(t, 0.0f, 1.0f);

	// Instantiate a new vector with lerped values
	return {
		x + (end.x - x) * t,
		y + (end.y - y) * t
	};
}

// Implementation for Math::Vector2::Lerp (t)
Vector2 Vector2::Lerp(float t)
{
	// Clamp 't' between 0 and 1
	t = std::clamp(t, 0.0f, 1.0f);

	// Instantiate a new vector with lerped values
	return {
		x * t,
		y * t
	};
}

// Implementation for Math::Vector2::Floor
Vector2 Vector2::Floor()
{
	return {
		std::floor(x),
		std::floor(y)
	};
}

// Implementation for Math::Vector2::Ceil
Vector2 Vector2::Ceil()
{
	return {
		std::ceil(x),
		std::ceil(y)
	};
}

// Implementation for Math::Vector2::Length
float Vector2::Length()
{
	// Calculation for euclidian length (magnitude)
	return sqrt(
		pow(x, 2) + pow(y, 2)
	);
}

// Implementation for Math::Vector2::Abs
Vector2 Vector2::Abs()
{
	return {
		fabs(x),
		fabs(y)
	};
}
// implementation for utils::W2S
Vector2 W2S(Vector3 origin, Matrix* view_matrix)
{
	Matrix temp;
		
	MatrixTranspose(&temp, view_matrix);

	Vector3 translationVector = Vector3(temp._41, temp._42, temp._43);
	Vector3 up = Vector3(temp._21, temp._22, temp._23);
	Vector3 right = Vector3(temp._11, temp._12, temp._13);

	float w = Vec3Dot(&translationVector, &origin) + temp._44;

	if (w < 0.098f)
		return { 0.0f, 0.0f };

	float y = Vec3Dot(&up, &origin) + temp._24;
	float x = Vec3Dot(&right, &origin) + temp._14;

	return Vector2
	{
		(globals->screen_size.x / 2) * (1.f + x / w),
		(globals->screen_size.y / 2) * (1.f - y / w)
	};
}

void Sleep(int64_t sleep_ms, std::chrono::time_point<std::chrono::steady_clock> start)
{
	// Convert the wanted ms sleep to us (Microseconds) 
	sleep_ms *= 1000;

	// Truncate the wanted time by the elapsed time 
	auto truncated = (sleep_ms - std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count()) / 1000;

	// Loop whilst the actual time is less than the wanted sleep time 
	while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() < sleep_ms)
	{
		// Use sleep until close to target then hot loop 
		if (truncated)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(truncated));
			truncated = 0;
		}
	}
}

Offsets* offsets = new Offsets();
Aimbot* aimbot = new Aimbot();
Features* features = new Features();