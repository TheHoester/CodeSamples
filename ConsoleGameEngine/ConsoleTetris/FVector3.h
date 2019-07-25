#pragma once
#include <math.h>

#include "Matrix4x4.h"

namespace Engine {
	namespace Physics {
		/**
		 * FVector3
		 */
		struct FVector3
		{
			float x;
			float y;
			float z;

			FVector3(void);
			FVector3(float x, float y, float z);

			// Functions
			float Magnitude(void) const;
			FVector3 Normalized(void) const;

			// Math Operators
			FVector3 operator+(const FVector3& other) const;
			FVector3 operator-(const FVector3& other) const;
			FVector3 operator*(const float& other) const;
			FVector3 operator*(const Matrix4x4& matrix) const;
			FVector3 operator/(const float& other) const;
			FVector3& operator=(const FVector3& other);
			FVector3& operator+=(const FVector3& other);
			FVector3& operator-=(const FVector3& other);
			FVector3& operator*=(const float& other);
			FVector3& operator*=(const Matrix4x4& matrix);
			FVector3& operator/=(const float& other);

			bool operator==(const FVector3& other) const;

			// Cross and Dot Product
			FVector3 CrossProduct(const FVector3& other) const;
			float DotProduct(const FVector3& other) const;
			
			// Misc Functions
			FVector3 IntersectPlane(FVector3& normal, const FVector3& lineStart, const FVector3& lineEnd);
		};
	}
}