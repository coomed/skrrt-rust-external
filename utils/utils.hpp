#ifndef _MATH_HPP
#define _MATH_HPP

// includes for this header
#include <cmath>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <future>

#define pCheck(pointer) if (IsValidPointer(pointer) != 1) return;

// undocumented NtDelayExecution function
static LONG(__stdcall* NtDelayExecution)(BOOL Alertable, PLARGE_INTEGER DelayInterval) = (LONG(__stdcall*)(BOOL, PLARGE_INTEGER)) GetProcAddress(GetModuleHandle("ntdll.dll"), "NtDelayExecution");

bool IsValidPointer(uint64_t address);

// wrapper around NtDelayExecution
inline bool Delay(int64_t ns)
{
	// Will store our relative interval for NtDelayExecution
	LARGE_INTEGER interval;

	// Store our relative delay time (multiply by -1 for relative)
	interval.QuadPart = -1 * ns;

	// Call NtDelayExecution and return success or failure
	return (NtDelayExecution(false, &interval) == 0); // 0 = STATUS_SUCCESS
}

constexpr unsigned int hash(const char* s, int off = 0) {
	return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}
// represents a 4x4 matrix
struct Matrix
{
	union
	{
		struct
		{
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};

struct Matrix3x4
{
	byte v0[16];
	byte v1[16];
	byte v2[16];
};

// used to transpose a 4x4 matrix
Matrix* MatrixTranspose(Matrix* pout, const Matrix* pm);

// a vector2 class
class Vector2
{
public:

	/*
	 * Used to initialize this class with no values
	 */
	Vector2();

	/*
	 * Used to initialize this class with given values
	 *
	 * Args:
	 *  x: the x-value for this vector2
	 *  y: the y-value for this vector2
	 *
	 * Returns:
	 *  A new vector2 containing your defined vlaues
	 */
	Vector2(float x, float y);

	/*
	 * Used to lerp between 0,0 and this vector's values
	 *
	 * Args:
	 *  t: A value between 0 and 1 representing the position in the lerp you want values for
	 *
	 * Returns:
	 *  The lerped values given t
	 */
	Vector2 Lerp(float t);

	/*
	 * Used to linearly interpolate between two vectors
	 *
	 * Args:
	 *  t: A value between 0 and 1 representing the position in the lerp you want values for
	 *  end: The vector you want to lerp between
	 *
	 * Returns:
	 *  The lerped values between this vector and 'end' vector given 't'
	 */
	Vector2 Lerp(Vector2 end, float t);

	/*
	 * Used to get the lower rounded natural numbers for this vector
	 *
	 * Returns:
	 *  A new vector containing natural numbers
	 */
	Vector2 Floor();

	/*
	 * Used to get the upper rounded natural numbers for this vector
	 *
	 * Returns:
	 *  A new vector containing natural numbers
	 */
	Vector2 Ceil();

	/*
	 * Convert a vector2 to it's absolute values
	 */
	Vector2 Abs();

	/*
	 * Used to get the euclidian length of a vector
	 */
	float Length();

	/*
	 * Overload Vector2 subtract operator
	 */
	Vector2 operator - (Vector2 other)
	{
		return {
			x - other.x,
			y - other.y
		};
	}

	/*
	 * Overload Vector2 addition operator
	 */
	Vector2 operator + (Vector2 other)
	{
		return {
			x + other.x,
			y + other.y
		};
	}

	/*
	 * Overload the Vector2 multiplication operator
	 */
	Vector2 operator * (Vector2 other)
	{
		return {
			x * other.x,
			y * other.y
		};
	}

	/*
	 * Overload for the Vector2 division operator
	 */
	Vector2 operator / (Vector2 other)
	{
		return {
			x / other.x,
			y / other.y
		};
	}

	/*
	 * Overload for the add assign operator
	 */
	void operator += (Vector2 other)
	{
		// Update this vector's values
		x += other.x;
		y += other.y;
	}

	/*
	 * Overload for the subtract assign operator
	 */
	void operator -= (Vector2 other)
	{
		// Update this vector's values
		x -= other.x;
		y -= other.y;
	}

	/*
	 * Overload for the multiply assign operator
	 */
	void operator *= (Vector2 other)
	{
		// Update this vector's values
		x *= other.x;
		y *= other.y;
	}

	/*
	 * Overload for the divide assign operator
	 */
	void operator /= (Vector2 other)
	{
		// Update this vector's values
		x /= other.x;
		y /= other.y;
	}

public: // Public fields for this class
	float x, y;
};

// clamp func
template<class T, class U>
T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

bool IsInCircle(Vector2 circle_pos, int rad, Vector2 point);

// a point class
struct Point
{
	float x, y;

	Point RotateAroundAnotherPoint(float angle, Point pivot)
	{
		float s = sin(angle);
		float c = cos(angle);

		x -= pivot.x;
		y -= pivot.y;

		float xnew = x * c - y * s;
		float ynew = x * s + y * c;

		x = xnew + pivot.x;
		y = ynew + pivot.y;

		return *this;
	}

};

// a triangle class
class Triangle
{
public:
	Point A;
	Point B;
	Point C;
	Point Center;

	void Build(float px, float py, float height = 20)
	{
		C.x = px;
		C.y = py - height;

		A.x = px - height / 3.0f;
		A.y = py;

		B.x = px + height / 3.0f;
		B.y = py;

		Center.x = (A.x + B.x + C.x) / 3;
		Center.y = (A.y + B.y + C.y) / 3;
	}

	void Rotate(float angle)
	{
		A.RotateAroundAnotherPoint(angle, Center);
		B.RotateAroundAnotherPoint(angle, Center);
		C.RotateAroundAnotherPoint(angle, Center);
	}
};

// a vector3 class
class Vector3
{
public:
	/* Default constructor for this class */
	Vector3()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	/* Constructor for this class */
	Vector3(float _x, float _y, float _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	/* Used to calculate the distance between two vector3s */
	float Distance(Vector3 b)
	{
		Vector3 calc(this->x - b.x, this->y - b.y, this->z - b.z);

		/* Calculate the distance */
		return sqrt(calc.x * calc.x + calc.y * calc.y + calc.z * calc.z);
	}

	/* Used to calculate the distance between two vector3s */
	float Distance2(Vector3 b)
	{
		Vector3 calc(this->x - b.x, this->y - b.y, this->z - b.z);

		/* Calculate the distance */
		return sqrtf(pow(calc.x, 2) + pow(calc.y, 2) + pow(calc.z, 2));
	}

	/* Used to get the magnitude of this vector */
	float Magnitude()
	{
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	/* Used to normalize this vector */
	Vector3 Normalized()
	{
		/* Get the magnitude of this vector */
		float num = this->Magnitude();

		/* Check if number is more than max. */
		if (num > 9.99999974737875E-06)
		{
			return { this->x / num, this->y / num, this->z / num };
		}

		/* Return an empty vector */
		return Vector3();
	}

	/* Used to calculate the dot product of this, and another vector */
	float Dot(Vector3 rhs)
	{
		return (this->x * rhs.x + this->y * rhs.y + this->z * rhs.z);
	}

	/* Used to calculate the normalized dot */
	float DotNormalized(Vector3 rhs)
	{
		/* Normalize both vectors */
		Vector3 lhs = this->Normalized();
		rhs = rhs.Normalized();

		return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
	}

	/* Returns the angle in degrees between this vec, and to */
	float Angle(Vector3 to)
	{
		return acos(clamp(-1.f, 1.f, this->DotNormalized(to))) * 57.29578f;
	}

public:

	/* Overload the + operator */
	Vector3 operator + (const Vector3& b)
	{
		return {
			this->x + b.x,
			this->y + b.y,
			this->z + b.z
		};
	}

	/* Overload the + operator */
	Vector3 operator + (float b)
	{
		return {
			this->x + b,
			this->y + b,
			this->z + b
		};
	}

	/* Overload the + operator */
	Vector3 operator - (const Vector3& b)
	{
		return {
			this->x - b.x,
			this->y - b.y,
			this->z - b.z
		};
	}

	/* Overload the - operator */
	Vector3 operator - (float b)
	{
		return {
			this->x - b,
			this->y - b,
			this->z - b
		};
	}

	/* Overload the + operator */
	Vector3 operator * (const Vector3& b)
	{
		return {
			this->x * b.x,
			this->y * b.y,
			this->z * b.z
		};
	}

	/* Overload the * perator */
	Vector3 operator * (float b)
	{
		return {
			this->x * b,
			this->y * b,
			this->z * b
		};
	}

	/* Overload the + operator */
	Vector3 operator / (const Vector3& b)
	{
		return {
			this->x / b.x,
			this->y / b.y,
			this->z / b.z
		};
	}

	/* Overload the / operator */
	Vector3 operator / (float b)
	{
		return {
			this->x / b,
			this->y / b,
			this->z / b
		};
	}

	/* Overload the + operator */
	void operator += (const Vector3& b)
	{

		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
	}

	/* Overload the + operator */
	void operator += (float b)
	{
		this->x += b;
		this->y += b;
		this->z += b;
	}

	/* Overload the + operator */
	void operator -= (const Vector3& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		this->z -= b.z;
	}

	/* Overload the + operator */
	void operator -= (float b)
	{

		this->x -= b;
		this->y -= b;
		this->z -= b;

	}

	/* Overload the + operator */
	void operator *= (const Vector3& b)
	{
		this->x *= b.x;
		this->y *= b.y;
		this->z *= b.z;
	}

	/* Overload the + operator */
	void operator *= (float b)
	{
		this->x *= b;
		this->y *= b;
		this->z *= b;
	}

	/* Overload the + operator */
	void operator /= (const Vector3& b)
	{
		this->x /= b.x;
		this->y /= b.y;
		this->z /= b.z;
	}

	/* Overload the + operator */
	void operator /= (float b)
	{
		this->x /= b;
		this->y /= b;
		this->z /= b;
	}

public:

	// Axial values
	float x;
	float y;
	float z;


};

// a vector4 class
class Vector4
{
public:
	/* Default constructor for this class */
	Vector4()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	/* Constructor for this class */
	Vector4(float _x, float _y, float _z, float _w)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
		this->w = _w;
	}
public:

	// Axial values
	float x;
	float y;
	float z;
	float w;
};

Vector3 CrossProduct(const Vector3& a, const Vector3& b);

Vector4 QuaternionLookRotation(Vector3 forward, Vector3 up);

// used to calculate the dot product of a vector3
float Vec3Dot(const Vector3* pV1, const Vector3* pV2);

Vector2 W2S(Vector3 origin, Matrix* view_matrix);


// Hot looping sleep function that uses timepoints to remain 'accurate'
void Sleep(int64_t sleep_ms, std::chrono::time_point<std::chrono::steady_clock> start);
#endif // _MATH_HPP