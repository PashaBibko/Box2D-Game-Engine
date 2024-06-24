#include <util/vec2.h>

#include <util/util.h>

// ----- Operator Overloads ----- //

// Addition operator overload

Vec2 operator+(Vec2& lhs, Vec2& rhs)
{
	lhs.x = lhs.x + rhs.x;
	lhs.y = lhs.y + rhs.y;

	return lhs;
}

// Subtraction operator overload

Vec2 operator-(Vec2& lhs, Vec2& rhs)
{
	lhs.x = lhs.x - rhs.x;
	lhs.y = lhs.y - rhs.y;

	return lhs;
}

// Multiplication operator overload

Vec2 operator*(Vec2& lhs, Vec2& rhs)
{
	lhs.x = lhs.x * rhs.x;
	lhs.y = lhs.y * rhs.y;

	return lhs;
}

// Division operator overload

Vec2 operator/(Vec2& lhs, Vec2& rhs)
{
	lhs.x = lhs.x / rhs.x;
	lhs.y = lhs.y / rhs.y;

	return lhs;
}

// Equality operator overload

bool operator==(Vec2& lhs, Vec2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

// Inequality operator overload

bool operator!=(Vec2& lhs, Vec2& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

// STD operator overloads

namespace std
{
	// Absolute function
	Vec2 abs(Vec2& vec)
	{
		return Vec2(abs(vec.x), abs(vec.y));
	}

	// Console output
	ostream& operator<<(ostream& os, const Vec2& vec)
	{
		os << "(" << vec.x << ", " << vec.y << ")";
		return os;
	}

	// To string
	string to_string(Vec2& vec)
	{
		return to_string(vec.x) + " " + to_string(vec.y);
	}

	// Min function
	Vec2 min(Vec2& lhs, Vec2& rhs)
	{
		return Vec2(min(lhs.x, rhs.x), min(lhs.y, rhs.y));
	}

	// Max function
	Vec2 max(Vec2& lhs, Vec2& rhs)
	{
		return Vec2(max(lhs.x, rhs.x), max(lhs.y, rhs.y));
	}

	// Floor function (rounds down)
	Vec2 floor(Vec2& vec)
	{
		return Vec2(floor(vec.x), floor(vec.y));
	}
}