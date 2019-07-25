#pragma once
#include <math.h>

#include "Defines.h"

namespace Engine { namespace Physics {
	struct FVector3;
	class Matrix4x4
	{
	public:
		float matrix[4][4];

		Matrix4x4(void);

		// Matrix Construction Functions
		static Matrix4x4& IdentityMatrix(void);
		static Matrix4x4& TranslationMatrix(const float& x, const float& y, const float& z);
		static Matrix4x4& RotationXMatrix(const float& angle);
		static Matrix4x4& RotationYMatrix(const float& angle);
		static Matrix4x4& RotationZMatrix(const float& angle);
		static Matrix4x4& ProjectionMatrix(const float& aspectRatio, const float& fov, const float& nearClipping, const float& farClipping);
		static Matrix4x4& PointAtMatrix(const FVector3& position, const FVector3& target, const FVector3& up);
		static Matrix4x4& InvertPointAtMatrix(const Matrix4x4& pointAt);

		// Operator Overloads
		Matrix4x4& operator*(const Matrix4x4& other) const;
		Matrix4x4& operator*=(const Matrix4x4& other);
	};
}}