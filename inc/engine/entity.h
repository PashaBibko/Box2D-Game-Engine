#pragma once

#include <engine/drawRect.h>
#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

/*
* @brief Defintion class for GraphicEntity
*/
struct GraphicDef
{
	Vec2 size;
	Vec2 position;
};

/*
* @brief Defintion class for PhysicalEntity
*/
struct PhysicalDef : public GraphicDef
{
	// B2BodyType is an enum that defines the type of the body:
	// - b2_staticBody: A body that does not move and is not affected by forces.
	// - b2_kinematicBody: A body that does not move and is not affected by forces, but can be moved manually.
	// - b2_dynamicBody: A body that moves and is affected by forces.
	b2BodyType bodyType;

	// A vector of vectors of Vec2 that defines the vertices of the fixtures.
	std::vector<std::vector<Vec2>> fixtureVertices;

	// Default constructor
	PhysicalDef() : bodyType(b2BodyType::b2_staticBody) { fixtureVertices.resize(0); }
};

/*
* @brief Base class for graphic entities
*/
class GraphicEntity : public Entity
{
	protected:
		// Drawable object of the entity
		drawRect drawable;

		/*
		* @brief True constructor of the class
		* 
		* @param call: If the constructor was called by the constructor of the class
		*/
		GraphicEntity(bool call);

	public:
		/*
		* @brief Default constructor of the class - Calls the true constructor with parameter true
		*/
		GraphicEntity() : GraphicEntity(true) {}

		/*
		* @brief Renders the entity
		*/
		void render() override;

		/*
		* @brief Creates a graphic entity and adds it to the instances vector
		* 
		* @param Definition of the entity
		* 
		* @return A shared pointer to the created entity
		*/
		static std::shared_ptr<GraphicEntity> create(GraphicDef def);
};

/*
* @brief Base class for physical entities
*/
class PhysicalEntity : public GraphicEntity
{
	public:
		// Pointer to the b2Body of the entity
		b2Body* body = nullptr;

		// Vector of shared pointers to the shapes of the fixtures
		std::vector<std::shared_ptr<b2Shape>> shapes;

		/*
		* @brief True constructor of the class
		* 
		* @param call: If the constructor was called by the constructor of the class
		*/
		PhysicalEntity(bool call);

	public:
		/*
		* @brief Default constructor of the class - Calls the true constructor with parameter true
		*/
		PhysicalEntity() : PhysicalEntity(true) {}

		/*
		* @brief Overriden update function of the entity
		*/
		void update() override;

		/*
		* @brief Overriden render function of the entity
		*/
		void render() override;

		/*
		* @brief Creates a physical entity and adds it to the instances vector
		* 
		* @param Definition of the entity
		* 
		* @return A shared pointer to the created entity
		*/
		static std::shared_ptr<PhysicalEntity> create(PhysicalDef def);
};