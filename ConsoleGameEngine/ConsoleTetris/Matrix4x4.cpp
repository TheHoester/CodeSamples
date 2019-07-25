#include "Matrix4x4.h"
#include "FVector3.h"

// CONSTRUCTOR #################################################################################################################################################

Engine::Physics::Matrix4x4::Matrix4x4()
{
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			matrix[y][x] = 0.0f;
}

// MATRIX CONSTRUCTION FUNCTIONS ###############################################################################################################################

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::IdentityMatrix()
{
	Matrix4x4 matrix;
	matrix.matrix[0][0] = 1.0f;
	matrix.matrix[1][1] = 1.0f;
	matrix.matrix[2][2] = 1.0f;
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::TranslationMatrix(const float& x, const float& y, const float& z)
{
	Matrix4x4 matrix;
	matrix.matrix[0][0] = 1.0f;
	matrix.matrix[1][1] = 1.0f;
	matrix.matrix[2][2] = 1.0f;
	matrix.matrix[3][3] = 1.0f;
	matrix.matrix[3][0] = x;
	matrix.matrix[3][1] = y;
	matrix.matrix[3][2] = z;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::RotationXMatrix(const float& angle)
{
	Matrix4x4 matrix;
	matrix.matrix[0][0] = 1.0f;
	matrix.matrix[1][1] = cosf(angle);
	matrix.matrix[1][2] = sinf(angle);
	matrix.matrix[2][1] = -sinf(angle);
	matrix.matrix[2][2] = cosf(angle);
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::RotationYMatrix(const float& angle)
{
	Matrix4x4 matrix;
	matrix.matrix[0][0] = cosf(angle);
	matrix.matrix[0][2] = sinf(angle);
	matrix.matrix[2][0] = -sinf(angle);
	matrix.matrix[1][1] = 1.0f;
	matrix.matrix[2][2] = cosf(angle);
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::RotationZMatrix(const float& angle)
{
	Matrix4x4 matrix;
	matrix.matrix[0][0] = cosf(angle);
	matrix.matrix[0][1] = sinf(angle);
	matrix.matrix[1][0] = -sinf(angle);
	matrix.matrix[1][1] = cosf(angle);
	matrix.matrix[2][2] = 1.0f;
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::ProjectionMatrix(const float& aspectRatio, const float& fov, const float& nearClipping, const float& farClipping)
{
	float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);
	Matrix4x4 matrix;
	matrix.matrix[0][0] = aspectRatio * fovRad;
	matrix.matrix[1][1] = fovRad;
	matrix.matrix[2][2] = farClipping / (farClipping - nearClipping);
	matrix.matrix[3][2] = (-farClipping * nearClipping) / (farClipping - nearClipping);
	matrix.matrix[2][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::PointAtMatrix(const FVector3& position, const FVector3& target, const FVector3& up)
{
	// Calculate forward, new up and right directions.
	FVector3 forward = (target - position).Normalized();
	FVector3 newUp = (up - (forward * up.DotProduct(forward))).Normalized();
	FVector3 right = newUp.CrossProduct(forward);

	// Construct point at matrix.
	Matrix4x4 matrix;
	matrix.matrix[0][0] = right.x;    matrix.matrix[0][1] = right.y;    matrix.matrix[0][2] = right.z;
	matrix.matrix[1][0] = newUp.x;    matrix.matrix[1][1] = newUp.y;    matrix.matrix[1][2] = newUp.z;
	matrix.matrix[2][0] = forward.x;  matrix.matrix[2][1] = forward.y;  matrix.matrix[2][2] = forward.z;
	matrix.matrix[3][0] = position.x; matrix.matrix[3][1] = position.y; matrix.matrix[3][2] = position.z;
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::InvertPointAtMatrix(const Matrix4x4& pointAt)
{
	// Construct point at matrix.
	Matrix4x4 matrix;
	matrix.matrix[0][0] = pointAt.matrix[0][0]; matrix.matrix[0][1] = pointAt.matrix[1][0]; matrix.matrix[0][2] = pointAt.matrix[2][0];
	matrix.matrix[1][0] = pointAt.matrix[0][1]; matrix.matrix[1][1] = pointAt.matrix[1][1]; matrix.matrix[1][2] = pointAt.matrix[2][1];
	matrix.matrix[2][0] = pointAt.matrix[0][2]; matrix.matrix[2][1] = pointAt.matrix[1][2]; matrix.matrix[2][2] = pointAt.matrix[2][2];
	matrix.matrix[3][0] = pointAt.matrix[0][3]; matrix.matrix[3][1] = pointAt.matrix[1][3]; matrix.matrix[3][2] = pointAt.matrix[2][3];
	matrix.matrix[3][0] = -(pointAt.matrix[3][0] * matrix.matrix[0][0] + pointAt.matrix[3][1] * matrix.matrix[1][0] + pointAt.matrix[3][2] * matrix.matrix[2][0]);
	matrix.matrix[3][1] = -(pointAt.matrix[3][0] * matrix.matrix[0][1] + pointAt.matrix[3][1] * matrix.matrix[1][1] + pointAt.matrix[3][2] * matrix.matrix[2][1]);
	matrix.matrix[3][2] = -(pointAt.matrix[3][0] * matrix.matrix[0][2] + pointAt.matrix[3][1] * matrix.matrix[1][2] + pointAt.matrix[3][2] * matrix.matrix[2][2]);
	matrix.matrix[3][3] = 1.0f;
	return matrix;
}

// OPERATOR OVERLOADS ##########################################################################################################################################

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::operator*(const Matrix4x4& other) const
{
	Matrix4x4 output;
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			output.matrix[y][x] = (matrix[y][0] * other.matrix[0][x]) + (matrix[y][1] * other.matrix[1][x]) + (matrix[y][2] * other.matrix[2][x]) + (matrix[y][3] * other.matrix[3][x]);

	return output;
}

Engine::Physics::Matrix4x4& Engine::Physics::Matrix4x4::operator*=(const Matrix4x4& other)
{
	for (int x = 0; x < 4; ++x)
		for (int y = 0; y < 4; ++y)
			matrix[y][x] = (matrix[y][0] * other.matrix[0][x]) + (matrix[y][1] * other.matrix[1][x]) + (matrix[y][2] * other.matrix[2][x]) + (matrix[y][3] * other.matrix[3][x]);

	return *this;
}