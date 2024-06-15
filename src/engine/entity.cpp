#include <engine/entity.h>

#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

// Entity vector (Defined in class but has to be created here)
std::vector<std::shared_ptr<Entity>> Entity::instances;

// --------------- Entity Member Functions --------------- //

void Entity::render() { /* E M P T Y */ }
void Entity::update() { /* E M P T Y */ }

Vec2 Entity::getSize()
{
	// Quality comment
	return size;
}

Vec2 Entity::getPosition()
{
	// No this comment is much better
	return position;
}

void Entity::setSize(Vec2 size)
{
	// Do I need a comment here?
	this->size = size;
}

void Entity::setPosition(Vec2 position)
{
	// No I don't need a comment here
	this->position = position;
}

// --------------- GraphicEntity Member Functions --------------- //

GraphicEntity::GraphicEntity(bool call)
{
	// If the call is false, it stops further initialization
	if (call == false)
		return;

	// Sets correct render state transform scale
	renderStates.transform.scale(Vec2(EngineInfo::scale));
}

void GraphicEntity::render()
{
	// Renders the drawable object to the Engine window
	EngineInfo::window->draw(drawable, renderStates);
}

// --------------- PhysicalEntity Member Functions --------------- //

PhysicalEntity::PhysicalEntity(bool call) : GraphicEntity(false)
{
	// If the call is false, it stops further initialization
	if (call == false)
		return;

	// Sets correct render state transform scale
	renderStates.transform.scale(Vec2(EngineInfo::scale));
}

PhysicalEntity::~PhysicalEntity()
{
	// Deletes the user data from the body
	delete reinterpret_cast<B2CustomUserData*>(body->GetUserData().pointer);
}

void PhysicalEntity::update()
{
	// Gets the position from the b2Body pointer
	position = Vec2(body->GetPosition());

	// Updates the position of the drawable object to the position of the physical object
	drawable.setPosition(position);
}

void PhysicalEntity::render()
{
	// Renders the drawable object to the Engine window
	EngineInfo::window->draw(drawable, renderStates);

	// Loops through all the shapes in the shapes vector
	for (size_t i = 0; i < shapes.size(); i++)
	{
		// Casts the shape pointer to a b2PolygonShape
		b2PolygonShape* shape = static_cast<b2PolygonShape*>(shapes[i].get());

		// Creates a new VertexArray object with the correct size and type
		sf::VertexArray hitbox(sf::LineStrip, shape->m_count + 1);

		// Loops through all the vertices in the shape and sets the position and color
		for (int32 j = 0; j < shape->m_count + 1; j++)
		{
			if (j == shape->m_count)
				hitbox[shape->m_count].position = Vec2(body->GetWorldPoint(shape->m_vertices[0]));

			else
				hitbox[j].position = Vec2(body->GetWorldPoint(shape->m_vertices[j]));

			hitbox[j].color = sf::Color::Red;
		}

		// Draws the hitbox to the Engine window
		EngineInfo::window->draw(hitbox, renderStates);
	}
}

void PhysicalEntity::setXVelocity(float x)
{
	//  Sets the x velocity of the body
	velocity.x = x;
}

void PhysicalEntity::setYVelocity(float y)
{
	// Sets the y velocity of the body
	velocity.y = y;
}

void PhysicalEntity::setVelocity(Vec2 velocity)
{
	// Sets the velocity of the body
	this->velocity = velocity;
}

void PhysicalEntity::addVelocity(Vec2 velocity)
{
	// Adds the velocity to the body
	this->velocity = this->velocity + velocity;
}

B2CustomUserData* PhysicalEntity::getB2UserData()
{
	// Returns the user data from the body
	return reinterpret_cast<B2CustomUserData*>(body->GetUserData().pointer);
}

// --------------- Creation functions for entities --------------- //

std::shared_ptr<GraphicEntity> GraphicEntity::create(GraphicDef def)
{
	// Creates a new instance of GraphicEntity and adds it to the instances vector
	instances.push_back(std::make_shared<GraphicEntity>());

	// Gets the last instance in the vector and casts it to a GraphicEntity
	std::shared_ptr<GraphicEntity> instance = std::static_pointer_cast<GraphicEntity>(instances.back());

	// Sets the type of the instance to GRAPHIC_ONLY
	instance->type = EntityType::GRAPHIC_ONLY;

	// Sets the size and position of the instance to the values in the def struct
	instance->size = def.size;
	instance->position = def.position;

	// Sets the drawable object to the parameters in the def struct
	instance->drawable.setHalfSize(def.size);
	instance->drawable.setPosition(def.position);

	// Returns the instance
	return instance;
}

std::shared_ptr<PhysicalEntity> PhysicalEntity::create(PhysicalDef def)
{
	// Creates a new instance of PhysicalEntity and adds it to the instances vector
	instances.push_back(std::make_shared<PhysicalEntity>());

	// Gets the last instance in the vector and casts it to a PhysicalEntity
	std::shared_ptr<PhysicalEntity> instance = std::static_pointer_cast<PhysicalEntity>(instances.back());

	// Sets the type of the instance to GRAPHIC_PHYSICAL
	instance->type = EntityType::GRAPHIC_PHYSICAL;

	// Sets the size and position of the instance to the values in the def struct
	instance->size = def.size;
	instance->position = def.position;

	// Sets the drawable object to the parameters in the def struct
	instance->drawable.setHalfSize(def.size);
	instance->drawable.setPosition(def.position);

	// Creates the body and sets the position and type

	b2BodyDef bodyDef;
	bodyDef.type = def.bodyType;
	bodyDef.position = def.position;

	instance->body = EngineInfo::world->CreateBody(&bodyDef);

	// Creates the shapes for the fixtures

	for (size_t i = 0; i < def.fixtureVertices.size(); i++)
	{
		// Creates a new b2PolygonShape object and a new b2Vec2 array
		b2PolygonShape shape;
		b2Vec2* vertices = new b2Vec2[def.fixtureVertices[i].size()];

		// Loops through all the vertices in the fixtureVertices vector and sets the values in the array
		for (size_t j = 0; j < def.fixtureVertices[i].size(); j++)
			vertices[j] = def.fixtureVertices[i][j];

		// Sets the shape to the vertices array and deletes the array
		shape.Set(vertices, def.fixtureVertices[i].size());
		delete[] vertices;

		// Adds the shape to the shapes vector
		instance->shapes.push_back(std::make_shared<b2PolygonShape>(shape));
	}

	// Creates the fixtures

	for (size_t i = 0; i < def.fixtureVertices.size(); i++)
	{
		// Creates a b2FixtureDef
		b2FixtureDef fixtureDef;
		
		// Assigns the created shape
		fixtureDef.shape = instance->shapes[i].get();

		// Assigns default values to the fixtureDef
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;
		fixtureDef.restitution = 0.0f;

		// Creates the fixture and assigns it to the body
		instance->body->CreateFixture(&fixtureDef);
	}

	// Creates a new B2CustomUserData object and assigns the instance to the owner
	B2CustomUserData* instanceUserData = new B2CustomUserData;
	instanceUserData->owner = instance;

	// Assigns the instanceUserData to the body's user data
	instance->body->GetUserData().pointer = reinterpret_cast<uintptr_t>(instanceUserData);

	// Returns the instance
	return instance;
}