#include "FVector3.h"

// CONSTRUCTORS ########################################################################################################################

Engine::Physics::FVector3::FVector3() : x(0.0f), y(0.0f), z(0.0f) { }
Engine::Physics::FVector3::FVector3(float x, float y, float z) : x(x), y(y), z(z) { }

// MATH FUNCTIONS ######################################################################################################################

float Engine::Physics::FVector3::Magnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}

Engine::Physics::FVector3 Engine::Physics::FVector3::Normalized() const
{
	float mag = Magnitude();
	if (mag == 0.0f)
		return FVector3(0.0f, 0.0f, 0.0f);
	else
		return *this / Magnitude();
}

// MATH OPERATORS ######################################################################################################################

Engine::Physics::FVector3 Engine::Physics::FVector3::operator+(const FVector3& other) const { return FVector3(x + other.x, y + other.y, z + other.z); }
Engine::Physics::FVector3 Engine::Physics::FVector3::operator-(const FVector3& other) const { return FVector3(x - other.x, y - other.y, z - other.z); }
Engine::Physics::FVector3 Engine::Physics::FVector3::operator*(const float& other) const { return FVector3(x * other, y * other, z * other); }

Engine::Physics::FVector3 Engine::Physics::FVector3::operator*(const Engine::Physics::Matrix4x4& matrix) const
{
	FVector3 output = FVector3();
	output.x = (x * matrix.matrix[0][0]) + (y * matrix.matrix[1][0]) + (z * matrix.matrix[2][0]) + matrix.matrix[3][0];
	output.y = (x * matrix.matrix[0][1]) + (y * matrix.matrix[1][1]) + (z * matrix.matrix[2][1]) + matrix.matrix[3][1];
	output.z = (x * matrix.matrix[0][2]) + (y * matrix.matrix[1][2]) + (z * matrix.matrix[2][2]) + matrix.matrix[3][2];
	float w =  (x * matrix.matrix[0][3]) + (y * matrix.matrix[1][3]) + (z * matrix.matrix[2][3]) + matrix.matrix[3][3];

	if (w != 0)
		output /= w;

	return output;
}

Engine::Physics::FVector3 Engine::Physics::FVector3::operator/(const float& other) const { return FVector3(x / other, y / other, z / other); }

Engine::Physics::FVector3& Engine::Physics::FVector3::operator=(const FVector3& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	return *this;
}

Engine::Physics::FVector3& Engine::Physics::FVector3::operator+=(const FVector3& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Engine::Physics::FVector3& Engine::Physics::FVector3::operator-=(const FVector3& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Engine::Physics::FVector3& Engine::Physics::FVector3::operator*=(const float& other)
{
	x *= other;
	y *= other;
	z *= other;
	return *this;
}

Engine::Physics::FVector3& Engine::Physics::FVector3::operator*=(const Physics::Matrix4x4& matrix)
{
	FVector3 output = FVector3();
	x = (x * matrix.matrix[0][0]) + (y * matrix.matrix[1][0]) + (z * matrix.matrix[2][0]) + matrix.matrix[3][0];
	y = (x * matrix.matrix[0][1]) + (y * matrix.matrix[1][1]) + (z * matrix.matrix[2][1]) + matrix.matrix[3][1];
	z = (x * matrix.matrix[0][2]) + (y * matrix.matrix[1][2]) + (z * matrix.matrix[2][2]) + matrix.matrix[3][2];
	float w = (x * matrix.matrix[0][3]) + (y * matrix.matrix[1][3]) + (z * matrix.matrix[2][3]) + matrix.matrix[3][3];

	if (w != 0)
		*this /= w;

	return *this;
}

Engine::Physics::FVector3& Engine::Physics::FVector3::operator/=(const float& other)
{
	x /= other;
	y /= other;
	z /= other;
	return *this;
}

bool Engine::Physics::FVector3::operator==(const FVector3& other) const
{
	return (x == other.x && y == other.y && z == other.z);
}

// CROSS AND DOT PRODUCT ###############################################################################################################################

Engine::Physics::FVector3 Engine::Physics::FVector3::CrossProduct(const FVector3& other) const
{
	FVector3 cross = FVector3(y * other.z - z * other.y, 
							   z * other.x - x * other.z, 
							   x * other.y - y * other.x);
	return cross;
}

float Engine::Physics::FVector3::DotProduct(const FVector3& other) const
{
	return (x * other.x) + (y * other.y) + (z * other.z);
}

// MISC FUNCTIONS ######################################################################################################################################

Engine::Physics::FVector3 Engine::Physics::FVector3::IntersectPlane(FVector3& normal, const FVector3& lineStart, const FVector3& lineEnd)
{
	normal = normal.Normalized();
	float planeDot = normal.DotProduct(*this);
	float startDot = lineStart.DotProduct(normal);
	float endDot = lineEnd.DotProduct(normal);
	float t = (planeDot - startDot) / (endDot - startDot);
	Physics::FVector3 intersectPoint = lineEnd - lineStart;
	intersectPoint *= t;
	intersectPoint += lineStart;
	return intersectPoint;
}