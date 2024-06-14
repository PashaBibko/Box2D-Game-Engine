#pragma once

#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

struct GraphicDef
{
	Vec2 size;
	Vec2 position;
};

class GraphicEntity : public Entity
{
	protected:
		// -- Variables -- //

		sf::RectangleShape drawable;

		GraphicEntity(bool call)
		{
			if (call == false)
				return;

			renderStates.transform.scale(Vec2(EngineInfo::scale));
		}

	public:
		// -- Constructor -- //
		GraphicEntity() : GraphicEntity(true) {}

		// -- Functions -- //
		void render() override
		{
			EngineInfo::window->draw(drawable, renderStates);
		}

		// -- Static Functions -- //
		static std::shared_ptr<GraphicEntity> create(GraphicDef def)
		{
			instances.push_back(std::make_shared<GraphicEntity>());
			std::shared_ptr<GraphicEntity> instance = std::static_pointer_cast<GraphicEntity>(instances.back());

			instance->type = EntityType::GRAPHIC_ONLY;

			instance->size = def.size;
			instance->position = def.position;

			instance->drawable.setSize(def.size);
			instance->drawable.setPosition(def.position);

			return instance;
		}
};

struct PhysicalDef
{
	Vec2 size;
	Vec2 position;

	b2BodyType bodyType;

	std::vector<std::vector<Vec2>> fixtureVertices;

	PhysicalDef() : bodyType(b2BodyType::b2_staticBody) { fixtureVertices.resize(0); }
};

class PhysicalEntity : public GraphicEntity
{
	public:
		// -- Variables -- //

		b2Body* body = nullptr;

		std::vector<std::shared_ptr<b2Shape>> shapes;

		PhysicalEntity(bool call) : GraphicEntity(false)
		{
			if (call == false)
				return;

			renderStates.transform.scale(Vec2(EngineInfo::scale));
		}

	public:
		// -- Constructor -- //
		PhysicalEntity() : PhysicalEntity(true) {}

		// -- Functions -- //

		void update() override
		{
			position = Vec2(body->GetPosition().x, body->GetPosition().y);

			drawable.setPosition(position);
		}

		void render() override
		{
			EngineInfo::window->draw(drawable, renderStates);

			for (size_t i = 0; i < shapes.size(); i++)
			{
				b2PolygonShape* shape = static_cast<b2PolygonShape*>(shapes[i].get());
				sf::VertexArray hitbox(sf::LineStrip, shape->m_count + 1);

				for (int32 j = 0; j < shape->m_count + 1; j++)
				{
					if (j == shape->m_count)
						hitbox[shape->m_count].position = Vec2(body->GetWorldPoint(shape->m_vertices[0]));

					else
						hitbox[j].position = Vec2(body->GetWorldPoint(shape->m_vertices[j]));

					hitbox[j].color = sf::Color::Red;
				}

				EngineInfo::window->draw(hitbox, renderStates);
			}
		}

		// -- Static Functions -- //

		static std::shared_ptr<PhysicalEntity> create(PhysicalDef def)
		{
			instances.push_back(std::make_shared<PhysicalEntity>());
			std::shared_ptr<PhysicalEntity> instance = std::static_pointer_cast<PhysicalEntity>(instances.back());

			instance->type = EntityType::GRAPHIC_PHYSICAL;

			instance->size = def.size;
			instance->position = def.position;

			instance->drawable.setSize(def.size);
			instance->drawable.setPosition(def.position);

			// Creats the body

			b2BodyDef bodyDef;
			bodyDef.type = def.bodyType;
			bodyDef.position = def.position;

			instance->body = EngineInfo::world->CreateBody(&bodyDef);

			// Creates the shapes for the fixtures

			for (size_t i = 0; i < def.fixtureVertices.size(); i++)
			{
				b2PolygonShape shape;
				b2Vec2* vertices = new b2Vec2[def.fixtureVertices[i].size()];

				for (size_t j = 0; j < def.fixtureVertices[i].size(); j++)
					vertices[j] = def.fixtureVertices[i][j];

				shape.Set(vertices, def.fixtureVertices[i].size());
				delete[] vertices;

				instance->shapes.push_back(std::make_shared<b2PolygonShape>(shape));
			}

			// Creates the fixtures

			for (size_t i = 0; i < def.fixtureVertices.size(); i++)
			{
				b2FixtureDef fixtureDef;
				fixtureDef.shape = instance->shapes[i].get();
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.0f;
				fixtureDef.restitution = 0.0f;

				instance->body->CreateFixture(&fixtureDef);
			}

			return instance;
		}
};