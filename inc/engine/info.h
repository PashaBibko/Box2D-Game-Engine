#pragma once

#include <util/util.h>

// Macros to turn _DEBUG into DEBUG
#ifdef _DEBUG
#define DEBUG true
#else
#define DEBUG false
#endif

/*
* @brief Contains all the important variables that are used throughout the engine
*/
struct EngineInfo
{
	// The window that the game is being rendered to
	static std::shared_ptr<sf::RenderWindow> window;

	// The b2World the game is simulating
	static std::shared_ptr<b2World> world;

	// Is the current window open
	static bool windowOpen;

	// Map of all the keys that are currently being pressed
	static std::unordered_map<sf::Keyboard::Key, long> keyMap;

	// Map of all the mouse buttons that are currently being pressed
	static std::unordered_map<sf::Mouse::Button, long> mouseMap;

	// The current mouse position
	static Vec2 mousePos;

	// Constant for converting between pixels and meters
	static const float scale;

	#if DEBUG

	// Debug variable for showing hitboxes
	static bool showHitboxes;

	#endif
};