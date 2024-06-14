#pragma once

#include <util/util.h>

/*
* @brief A class that draws a rectangle
*/
class drawRect : public sf::Drawable
{
	private:
		// Half size of the rectangle
		Vec2 halfSize;

		// Position of the rectangle
		Vec2 position;

		// Vertices of the rectangle
		sf::VertexArray vertices;

		/*
		* @brief Updates the vertices of the rectangle
		*/
		void updateVertices();

	public:
		/*
		* @brief Constructor
		* 
		* @param halfSize Half size of the rectangle
		* @param position Position of the rectangle
		*/
		drawRect(Vec2 halfSize = Vec2(0), Vec2 position = Vec2(0));

		/*
		* @brief Sets the half size of the rectangle
		* 
		* @param size New half size of the rectangle
		*/
		void setHalfSize(Vec2 size);

		/*
		* @brief Sets the position of the rectangle
		* 
		* @param position New position of the rectangle
		*/
		void setPosition(Vec2 position);

		/*
		* @brief Gets the half size of the rectangle
		*/
		Vec2 getHalfSize();

		/*
		* @brief Gets the position of the rectangle
		*/
		Vec2 getPosition();

		/*
		* @brief Draws the rectangle
		* 
		* @param target SFML Render target
		* @param states SFML Render states
		*/
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};