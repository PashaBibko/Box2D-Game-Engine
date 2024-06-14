#pragma once

#include <util/util.h>

#ifdef _DEBUG
#define DEBUG true
#else
#define DEBUG false
#endif

struct EngineInfo
{
	static std::shared_ptr<sf::RenderWindow> window;
	static std::shared_ptr<b2World> world;

	static bool windowOpen;

	static std::unordered_map<sf::Keyboard::Key, long> keyMap;
	static std::unordered_map<sf::Mouse::Button, long> mouseMap;

	static Vec2 mousePos;

	static const float scale;

	#if DEBUG

	static bool showHitboxes;

	#endif
};