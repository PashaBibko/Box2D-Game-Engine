#pragma once

#include <engine/base.h>

#include <util/libs.h>
#include <util/vec2.h>

enum class EntityType
{
	GRAPHIC_ONLY,

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
		static std::shared_ptr<GraphicEntity> create(Vec2 size, Vec2 position)
		{
			instances.push_back(std::make_shared<GraphicEntity>());
			std::shared_ptr<GraphicEntity> instance = std::static_pointer_cast<GraphicEntity>(instances.back());

			instance->type = EntityType::GRAPHIC_ONLY;

			instance->size = size;
			instance->position = position;

			instance->drawable.setSize(sf::Vector2f(size.x, size.y));
			instance->drawable.setPosition(sf::Vector2f(position.x, position.y));

			return instance;
		}
};