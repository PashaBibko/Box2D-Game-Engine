#pragma once

#include <engine/drawRect.h>
#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

// Foward declaration of the B2CustomUserData class
struct B2CustomUserData;

/*
* @brief Converts a string to a b2BodyType
* 
* @param str: String to convert
* 
* @return The converted b2BodyType
*/
b2BodyType convertFromStr(const std::string& str);

/*
* @brief Converts a b2BodyType to a string
* 
* @param type: b2BodyType to convert
* 
* @return The converted string
*/
std::string convertToStr(b2BodyType type);

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
		// Friends the def creation function
		friend GraphicDef createDefOf(GraphicEntity* entity);

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
		static GraphicEntity* create(GraphicDef def);
};

/*
* @brief Base class for physical entities
*/
class PhysicalEntity : public GraphicEntity
{
	private:
		// Friends the engine class to allow it to access it's private members
		friend class Engine;
		 
		// Friends the def creation function
		friend PhysicalDef createDefOf(PhysicalEntity* entity);

		// Pointer to the b2Body of the entity
		b2Body* body = nullptr;

		// Vector of shared pointers to the shapes of the fixtures
		std::vector<std::shared_ptr<b2Shape>> shapes;

		// Velocity of the entity
		Vec2 velocity;

		/**/
		void preStepUpdate() override;

		/**/
		void postStepUpdate() override;

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
		* @brief Destructor of the class - Destroys the userData of the b2Body to avoid memory leaks
		*/
		~PhysicalEntity();

		/*
		* @brief Overriden render function of the entity
		*/
		void render() override;

		/*
		* @brief Sets the velocity of the entity
		* 
		* @param x: New x velocity
		*/
		void setXVelocity(float x);

		/*
		* @brief Sets the velocity of the entity
		* 
		* @param x: New x velocity
		*/
		void setYVelocity(float y);

		/*
		* @brief Sets the velocity of the entity
		* 
		* @param velocity: New velocity
		*/
		void setVelocity(Vec2 velocity);

		/*
		* @brief Adds velocity to the entity
		* 
		* @param velocity: Velocity to add
		*/
		void addVelocity(Vec2 velocity);

		/*
		* @brief Gets the user data of the b2Body
		*/
		B2CustomUserData* getB2UserData();

		/*
		* @brief Creates a physical entity and adds it to the instances vector
		* 
		* @param Definition of the entity
		* 
		* @return A shared pointer to the created entity
		*/
		static PhysicalEntity* create(PhysicalDef def);
};

/*
* @brief Creates a definition for a graphic entity
* 
* @param entity: Entity to create the definition for
* 
* @return Definition of the entity
*/
GraphicDef createDefOf(GraphicEntity* entity);

/*
* @brief Creates a definition for a physical entity
* 
* @param entity: Entity to create the definition for
* 
* @return Definition of the entity
*/
PhysicalDef createDefOf(PhysicalEntity* entity);