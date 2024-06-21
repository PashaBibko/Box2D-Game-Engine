#pragma once

// Includes all libraries and headers needed for the project

// External Libraries:
// - SFML: Simple and Fast Multimedia Library (graphics, audio and input)
// - Box2D: 2D Physics Engine
// - JSON parser for Modern C++

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <Box2D/Box2D.h>

#include <json.hpp>

// Overides namespaces

namespace nl = nlohmann;

// Standard Libraries

#include <unordered_map>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>