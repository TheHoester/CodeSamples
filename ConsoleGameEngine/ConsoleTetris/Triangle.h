#pragma once
#include "FVector3.h"
#include "Colour.h"

namespace Engine { namespace Graphics {
	class Triangle
	{
	public:
		Physics::FVector3 points[3];
		short pixel;
		short colour;

		Triangle(void);
		Triangle(Physics::FVector3 first, Physics::FVector3 second, Physics::FVector3 third);

		// Operator Overloads
		Triangle& operator=(const Triangle&other);
		Triangle& operator+(const float& other) const;
		Triangle& operator+(const Physics::FVector3& other) const;
		Triangle& operator*(const float& other) const;
		Triangle& operator*(const Physics::FVector3& other) const;
		Triangle& operator*(const Physics::Matrix4x4& other) const;
		Triangle& operator+=(const float& other);
		Triangle& operator+=(const Physics::FVector3& other);
		Triangle& operator*=(const float& other);
		Triangle& operator*=(const Physics::FVector3& other);
		Triangle& operator*=(const Physics::Matrix4x4& other);
		
		// Misc Functions
		int ClipAgainstPlane(Physics::FVector3 planePoint, Physics::FVector3 planeNormal, Triangle& outOne, Triangle& outTwo);
	};
} }