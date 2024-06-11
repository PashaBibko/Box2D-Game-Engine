#pragma once

#include <util/libs.h>

struct Vec2
{
	// Member vars
	float x, y;

	// ----- Constructors ----- // 

	// Simple constructors
	Vec2() : x(0), y(0) {}
	Vec2(float xy) : x(xy), y(xy) {}
	Vec2(float x, float y) : x(x), y(y) {}

	// SFML conversion - sf::Vector2f
	Vec2(sf::Vector2f vec) : x(vec.x), y(vec.y) {}
	operator sf::Vector2f() { return sf::Vector2f(x, y); }

	// SFML conversion - sf::Vector2i
	Vec2(sf::Vector2i vec) : x((float)vec.x), y((float)vec.y) {}
	operator sf::Vector2i() { return sf::Vector2i((int)x, (int)y); }

	// Box2D conversion - b2Vec2
	Vec2 (b2Vec2 vec) : x(vec.x), y(vec.y) {}
	operator b2Vec2() { return b2Vec2(x, y); }

	// ----- Void Functions ----- //

	// Sets the vector to zero
	void setZero() { x = 0; y = 0; }

	// Sets the vector to the numeric limit
	void setInf()
	{
		x = std::numeric_limits<float>::max();
		y = std::numeric_limits<float>::max();
	}

	// Sets the vector to the negative numeric limit
	void setNegInf()
	{
		x = std::numeric_limits<float>::min();
		y = std::numeric_limits<float>::min();
	}

	// ----- Output Functions ----- //

	// Gets the length of the vector
	float length() { return sqrt(x * x + y * y); }

	// Gets the squared length of the vector (useful for performance)
	float lengthSquared() { return x * x + y * y; }
};

// ----- Operator Overloads ----- //

// Addition operator overload

Vec2 operator+(Vec2& lhs, const Vec2& rhs);

Vec2 operator-(Vec2& lhs, const Vec2& rhs);

Vec2 operator*(Vec2& lhs, const Vec2& rhs);

Vec2 operator/(Vec2& lhs, const Vec2& rhs);

bool operator==(Vec2& lhs, const Vec2& rhs);

bool operator!=(Vec2& lhs, const Vec2& rhs);

// STD operator overloads

namespace std
{
	Vec2 abs(Vec2& vec);

	ostream& operator<<(ostream& os, const Vec2& vec);

	string to_string(Vec2& vec);

	Vec2 min(Vec2& lhs, Vec2& rhs);

	Vec2 max(Vec2& lhs, Vec2& rhs);

	Vec2 floor(Vec2& vec);
}