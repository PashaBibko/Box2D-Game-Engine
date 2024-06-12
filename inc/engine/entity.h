#pragma once

#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

enum class EntityType
{
	GRAPHIC_ONLY,
	GRAPHIC_PHYSICAL,

	UNDEFINED
};

class Entity
{
	public:
		// -- Static members -- //
		static std::vector<std::shared_ptr<Entity>> instances;

		// -- Virtual functions -- //

		virtual void update();

		// -- Variables -- //

		EntityType type = EntityType::UNDEFINED;

		Vec2 size;
		Vec2 position;

	public:
		// -- Virtual functions -- //

		virtual void render();

		// -- Functions -- //

		Vec2 getSize();
		Vec2 getPosition();

		void setSize(Vec2 size);
		void setPosition(Vec2 position);
};

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

	public:
		// -- Functions -- //
		void render() override
		{
			EngineInfo::window->draw(drawable);
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

	std::vector<Vec2> vertices;

	PhysicalDef() : bodyType(b2BodyType::b2_staticBody) { vertices.resize(0); }
};

class PhysicalEntity : public GraphicEntity
{
	public:
		// -- Variables -- //

		b2Body* body = nullptr;

		std::vector<std::shared_ptr<b2Shape>> shapes;

	public:
		// -- Functions -- //

		void update() override
		{
			position = Vec2(body->GetPosition().x, body->GetPosition().y);
		}

		void render() override
		{
			EngineInfo::window->draw(drawable);

			for (int i = 0; i < shapes.size(); i++)
			{
				std::shared_ptr<b2PolygonShape>shape = std::static_pointer_cast<b2PolygonShape>(shapes[i]);
				sf::VertexArray hitbox(sf::LineStrip, shape->m_count + 1);

				for (int j = 0; j < shape->m_count + 1; j++)
				{
					if (j == shape->m_count)
						hitbox[shape->m_count].position = Vec2(body->GetWorldPoint(shape->m_vertices[0]));

					else
						hitbox[j].position = Vec2(body->GetWorldPoint(shape->m_vertices[j]));

					hitbox[j].color = sf::Color::Red;
				}

				EngineInfo::window->draw(hitbox);
			}

			sf::VertexArray line(sf::LineStrip, 2);
			line[0].position = position;
			line[1].position = Vec2(250, 250);

			EngineInfo::window->draw(line);
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

			//

			b2BodyDef bodyDef;
			bodyDef.type = def.bodyType;
			bodyDef.position = def.position;

			instance->body = EngineInfo::world->CreateBody(&bodyDef);

			b2PolygonShape shape;
			b2Vec2* vertices = new b2Vec2[def.vertices.size()];

			if (def.vertices.size() == 0)
				throw std::runtime_error("PhysicalEntity::create: def.vertices.size() == 0");

			for (size_t i = 0; i < def.vertices.size(); i++)
				vertices[i] = def.vertices[i];

			shape.Set(vertices, def.vertices.size());
			delete[] vertices;

			instance->shapes.push_back(std::make_shared<b2PolygonShape>(shape));

			b2FixtureDef fixtureDef;
			fixtureDef.shape = instance->shapes[0].get();
			fixtureDef.density = 1.0f;
			fixtureDef.friction = 0.0f;
			fixtureDef.restitution = 0.0f;

			instance->body->CreateFixture(&fixtureDef);

			return instance;
		}
};