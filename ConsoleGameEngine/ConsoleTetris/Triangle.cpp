#include "Triangle.h"

// CONSTRUCTORS ###################################################################################################################

Engine::Graphics::Triangle::Triangle() : Triangle(Physics::FVector3(), Physics::FVector3(), Physics::FVector3()) { }
Engine::Graphics::Triangle::Triangle(Physics::FVector3 first, Physics::FVector3 second, Physics::FVector3 third)
{
	points[0] = first;
	points[1] = second;
	points[2] = third;

	pixel = 0;
	colour = 0;
}

// OPERTATOR OVERLOADS ############################################################################################################

// ASSIGN = 
Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator=(const Triangle& other)
{
	points[0] = other.points[0];
	points[1] = other.points[1];
	points[2] = other.points[2];
	pixel = other.pixel;
	colour = other.colour;
	return *this;
}

// PLUS +
Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator+(const float& other) const
{
	Triangle temp;
	temp.points[0].x = points[0].x + other;
	temp.points[1].x = points[1].x + other;
	temp.points[2].x = points[2].x + other;
	temp.points[0].y = points[0].y + other;
	temp.points[1].y = points[1].y + other;
	temp.points[2].y = points[2].y + other;
	temp.points[0].z = points[0].z + other;
	temp.points[1].z = points[1].z + other;
	temp.points[2].z = points[2].z + other;
	temp.pixel = pixel;
	temp.colour = colour;
	return temp;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator+(const Physics::FVector3& other) const
{
	Triangle temp;
	temp.points[0] = points[0] + other;
	temp.points[1] = points[1] + other;
	temp.points[2] = points[2] + other;
	temp.pixel = pixel;
	temp.colour = colour;
	return temp;
}

// TIMES *
Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*(const float& other) const
{
	Triangle temp;
	temp.points[0].x = points[0].x * other;
	temp.points[1].x = points[1].x * other;
	temp.points[2].x = points[2].x * other;
	temp.points[0].y = points[0].y * other;
	temp.points[1].y = points[1].y * other;
	temp.points[2].y = points[2].y * other;
	temp.points[0].z = points[0].z * other;
	temp.points[1].z = points[1].z * other;
	temp.points[2].z = points[2].z * other;
	temp.pixel = pixel;
	temp.colour = colour;
	return temp;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*(const Physics::FVector3& other) const
{
	Triangle temp;
	temp.points[0].x = points[0].x * other.x;
	temp.points[1].x = points[1].x * other.x;
	temp.points[2].x = points[2].x * other.x;
	temp.points[0].y = points[0].y * other.y;
	temp.points[1].y = points[1].y * other.y;
	temp.points[2].y = points[2].y * other.y;
	temp.points[0].z = points[0].z * other.z;
	temp.points[1].z = points[1].z * other.z;
	temp.points[2].z = points[2].z * other.z;
	temp.pixel = pixel;
	temp.colour = colour;
	return temp;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*(const Physics::Matrix4x4& other) const
{
	Triangle temp;
	temp.points[0] = points[0] * other;
	temp.points[1] = points[1] * other;
	temp.points[2] = points[2] * other;
	temp.pixel = pixel;
	temp.colour = colour;
	return temp;
}

// PLUS AND ASSIGN +=
Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator+=(const float& other)
{
	points[0].x += other;
	points[1].x += other;
	points[2].x += other;
	points[0].y += other;
	points[1].y += other;
	points[2].y += other;
	points[0].z += other;
	points[1].z += other;
	points[2].z += other;
	return *this;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator+=(const Physics::FVector3& other)
{
	points[0] += other;
	points[1] += other;
	points[2] += other;
	return *this;
}

// TIMES AND ASSIGN *=
Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*=(const float& other)
{
	points[0].x *= other;
	points[1].x *= other;
	points[2].x *= other;
	points[0].y *= other;
	points[1].y *= other;
	points[2].y *= other;
	points[0].z *= other;
	points[1].z *= other;
	points[2].z *= other;
	return *this;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*=(const Physics::FVector3& other)
{
	points[0].x *= other.x;
	points[1].x *= other.x;
	points[2].x *= other.x;
	points[0].y *= other.y;
	points[1].y *= other.y;
	points[2].y *= other.y;
	points[0].z *= other.z;
	points[1].z *= other.z;
	points[2].z *= other.z;
	return *this;
}

Engine::Graphics::Triangle& Engine::Graphics::Triangle::operator*=(const Physics::Matrix4x4& other)
{
	points[0] *= other;
	points[1] *= other;
	points[2] *= other;
	return *this;
}

// MISC FUNCTIONS #################################################################################################################

int Engine::Graphics::Triangle::ClipAgainstPlane(Physics::FVector3 planePoint, Physics::FVector3 planeNormal, Triangle& outOne, Triangle& outTwo)
{
	planeNormal = planeNormal.Normalized();

	// Return signed shortest distance from point to plane
	auto distance = [&](Physics::FVector3 &point)
	{
		return (planeNormal.DotProduct(point) - planeNormal.DotProduct(planePoint));
	};

	// Create two temporary storage arrays to classify points either side of plane
	Physics::FVector3* insidePoints[3];  int insidePointCount = 0;
	Physics::FVector3* outsidePoints[3]; int outsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = distance(points[0]);
	float d1 = distance(points[1]);
	float d2 = distance(points[2]);
	
	// Checks if a point is infront or behind the plane
	if (d0 >= 0) insidePoints[insidePointCount++] = &points[0];
	else outsidePoints[outsidePointCount++] = &points[0];
	if (d1 >= 0) insidePoints[insidePointCount++] = &points[1];
	else outsidePoints[outsidePointCount++] = &points[1];
	if (d2 >= 0) insidePoints[insidePointCount++] = &points[2];
	else outsidePoints[outsidePointCount++] = &points[2];

	// Triangle is outside the plane and can be ignored
	if (insidePointCount == 0)
		return 0;

	// Triange is inside the plane and doesn't need clipping
	if (insidePointCount == 3)
	{
		outOne = *this;
		return 1;
	}

	// Only one point inside so only one clipped triangle is needed
	if (insidePointCount == 1 && outsidePointCount == 2)
	{
		outOne.pixel = pixel;
		outOne.colour = colour;
		/*outOne.pixel = PIXEL_SOLID;
		outOne.colour = FG_BLUE;*/

		// Inside point is valid
		outOne.points[0] = *insidePoints[0];

		// Find where the other two points intersect the plane
		outOne.points[1] = planePoint.IntersectPlane(planeNormal, *insidePoints[0], *outsidePoints[0]);
		outOne.points[2] = planePoint.IntersectPlane(planeNormal, *insidePoints[0], *outsidePoints[1]);

		return 1;
	}

	// Two points inside so two new trianglea are needed to clip
	if (insidePointCount == 2 && outsidePointCount == 1)
	{
		outOne.pixel = pixel;
		outOne.colour = colour;
		outTwo.pixel = pixel;
		outTwo.colour = colour;
		/*outOne.pixel = PIXEL_SOLID;
		outOne.colour = FG_RED;
		outTwo.pixel = PIXEL_SOLID;
		outTwo.colour = FG_GREEN;*/

		// First Triangle
		outOne.points[0] = *insidePoints[0];
		outOne.points[1] = *insidePoints[1];
		outOne.points[2] = planePoint.IntersectPlane(planeNormal, *insidePoints[0], *outsidePoints[0]);

		// Second Triangle
		outTwo.points[0] = *insidePoints[1];
		outTwo.points[1] = outOne.points[2];
		outTwo.points[2] = planePoint.IntersectPlane(planeNormal, *insidePoints[1], *outsidePoints[0]);

		return 2;
	}
}