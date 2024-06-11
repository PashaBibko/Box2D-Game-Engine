#pragma once

#include <util/util.h>

#include <engine/info.h>

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