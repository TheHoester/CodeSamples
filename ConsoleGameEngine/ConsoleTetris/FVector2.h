#pragma once
#include <math.h>

namespace Engine { namespace Physics {
	/**
	 * FVector2
	 */
	struct FVector2
	{
		float x;
		float y;

		FVector2(void);
		FVector2(float x, float y);

		// Functions
		float Magnitude(void) const;
		FVector2 Normalized(void) const;

		// Math Operators
		FVector2 operator+(const FVector2& other) const;
		FVector2 operator-(const FVector2& other) const;
		FVector2 operator*(const float& other) const;
		FVector2 operator/(const float& other) const;
		FVector2 operator=(const FVector2& other);
		FVector2 operator+=(const FVector2& other);
		FVector2 operator-=(const FVector2& other);
		FVector2 operator*=(const float& other);
		FVector2 operator/=(const float& other);

		bool operator==(const FVector2& other) const;
	};
} }