#pragma once

#include <util/util.h>

#include <engine/info.h>

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

		sf::RenderStates renderStates;

	public:
		// -- Virtual functions -- //

		virtual void render();

		// -- Functions -- //

		Vec2 getSize();
		Vec2 getPosition();

		void setSize(Vec2 size);
		void setPosition(Vec2 position);
};

class EngineController
{
	public:
		EngineController() {}
		virtual ~EngineController() {}

		virtual void onInit() {}
		virtual void onRender() {}
		virtual void onUpdate() {}
};

class Engine
{
	public:
		std::unique_ptr<EngineController>controller;

		Engine(Vec2 windowSize, std::unique_ptr<EngineController>controller = nullptr, bool fullscreen = false);

		void update();
		void render();
};

#define BASIC_LOOP(instance) \
while (EngineInfo::windowOpen) \
{ \
	instance.update(); \
	instance.render(); \
}