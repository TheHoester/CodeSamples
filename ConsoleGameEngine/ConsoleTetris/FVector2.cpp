#include "FVector2.h"

// CONSTRUCTORS ########################################################################################################################

FVector2::FVector2() : x(0.0f), y(0.0f) { }
FVector2::FVector2(float x, float y) : x(x), y(y) { }

// MATH FUNCTIONS ######################################################################################################################

float FVector2::Magnitude() const
{
	return sqrt((x * x) + (y * y));
}

FVector2 FVector2::Normalized() const
{
	return *this / Magnitude();
}

// MATH OPERATORS ######################################################################################################################

FVector2 FVector2::operator+(const FVector2 other) const { return FVector2(x + other.x, y + other.y); }
FVector2 FVector2::operator-(const FVector2 other) const { return FVector2(x - other.x, y - other.y); }
FVector2 FVector2::operator*(const float other) const {	return FVector2(x * other, y * other); }
FVector2 FVector2::operator/(const float other) const { return FVector2(x / other, y / other); }

FVector2 FVector2::operator=(const FVector2 other)
{
	x = other.x;
	y = other.y;
	return *this;
}

FVector2 FVector2::operator+=(const FVector2 other)
{
	x += other.x;
	y += other.y;
	return *this;
}

FVector2 FVector2::operator-=(const FVector2 other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

FVector2 FVector2::operator*=(const float other)
{
	x *= other;
	y *= other;
	return *this;
}

FVector2 FVector2::operator/=(const float other)
{
	x /= other;
	y /= other;
	return *this;
}

bool FVector2::operator==(const FVector2 other) const
{ 
	return (x == other.x && y == other.y); 
}