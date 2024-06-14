#include <engine/drawRect.h>

#include <util/util.h>

void drawRect::updateVertices()
{
	// Set the vertices of the rectangle
	vertices[0].position = Vec2(position.x - halfSize.x, position.y - halfSize.y);
	vertices[1].position = Vec2(position.x + halfSize.x, position.y - halfSize.y);
	vertices[2].position = Vec2(position.x - halfSize.x, position.y + halfSize.y);
	vertices[3].position = Vec2(position.x + halfSize.x, position.y + halfSize.y);
}

drawRect::drawRect(Vec2 halfSize, Vec2 position) : halfSize(halfSize), position(position)
{
	// Sets the information of the vertex array
	vertices.setPrimitiveType(sf::TriangleStrip);
	vertices.resize(4);

	// Set the vertices of the rectangle because parameters have changed
	updateVertices();
}

void drawRect::setHalfSize(Vec2 size)
{
	// Set the half size of the rectangle
	halfSize = size;

	// Update the vertices of the rectangle
	updateVertices();
}

void drawRect::setPosition(Vec2 position)
{
	// Set the position of the rectangle
	this->position = position;

	// Update the vertices of the rectangle
	updateVertices();
}

Vec2 drawRect::getHalfSize()
{
	// Return the half size of the rectangle
	return halfSize;
}

Vec2 drawRect::getPosition()
{
	// Return the position of the rectangle
	return position;
}

void drawRect::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Draw the rectangle
	target.draw(vertices, states);
}
