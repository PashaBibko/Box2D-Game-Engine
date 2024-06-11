#include <engine/base.h>

#include <util/util.h>

#include <engine/info.h>

// Creates instances of static members of EngineInfo

std::shared_ptr<sf::RenderWindow> EngineInfo::window = nullptr;
std::shared_ptr<b2World> EngineInfo::world = nullptr;

bool EngineInfo::windowOpen = false;

std::unordered_map<sf::Keyboard::Key, short> EngineInfo::keyMap;
std::unordered_map<sf::Mouse::Button, short> EngineInfo::mouseMap;

Vec2 EngineInfo::mousePos;

#if DEBUG

bool EngineInfo::showHitboxes = false;

#endif

// ----- Engine Functions ----- //

Engine::Engine(Vec2 windowSize, std::unique_ptr<EngineController>controller, bool fullscreen) : controller(std::move(controller))
{
	// ----- Instializes the members of EngineInfo ----- //
	
	// Creates the window
	sf::VideoMode videoMode = (fullscreen) ? sf::VideoMode::getDesktopMode() : sf::VideoMode((int)windowSize.x, (int)windowSize.y);

	EngineInfo::window = std::make_shared<sf::RenderWindow>(videoMode, "GAME ENGINE");
	EngineInfo::window->setView(sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y)));
	EngineInfo::window->setFramerateLimit(60);

	// Sets the windowOpen flag
	EngineInfo::windowOpen = EngineInfo::window->isOpen();

	// Creates the box2d world
	EngineInfo::world = std::make_shared<b2World>(Vec2(0.0f));

	// Calls the init controller function if it exists
	if (this->controller != nullptr)
		this->controller->onInit();
}

void Engine::update()
{
	// ----- Checks window and b2World are not nullptrs ----- //
	
	if (EngineInfo::window == nullptr || EngineInfo::world == nullptr)
		throw std::runtime_error("EngineInfo::window or EngineInfo::world is nullptr");

	// ----- Polls window events ----- //

	sf::Event event;

	while (EngineInfo::window->pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::Closed:
				EngineInfo::window->close();
				EngineInfo::windowOpen = EngineInfo::window->isOpen();
				break;

			case sf::Event::MouseMoved:
				EngineInfo::mousePos = EngineInfo::window->mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
				break;
		}
	}

	// ----- Updates the keyMap and mouseMap ----- //

	callFuncOnMap (EngineInfo::keyMap, [](sf::Keyboard::Key key, short& value)
		{ sf::Keyboard::isKeyPressed(key) ? ((value < 0) ? value = 0 : value++) : ((value > 0) ? value = 0 : value--); });

	callFuncOnMap (EngineInfo::mouseMap, [](sf::Mouse::Button button, short& value)
		{ sf::Mouse::isButtonPressed(button) ? ((value < 0) ? value = 0 : value++) : ((value > 0) ? value = 0 : value--); });

	// ----- Updates the b2World ----- //

	#define VELOCITY_ITERATIONS 8
	#define POSITION_ITERATIONS 3

	EngineInfo::world->Step(1.0f / 60.0f, 8, 3);

	// Calls the update controller function if it exists
	if (controller != nullptr)
		controller->onUpdate();
}

void Engine::render()
{
	// ----- Checks window is not nullptr ----- //
	if (EngineInfo::window == nullptr)
		throw std::runtime_error("EngineInfo::window is nullptr");

	// Clears the window
	EngineInfo::window->clear();

	// Calls the render controller function if it exists
	if (controller != nullptr)
		controller->onRender();

	// Displays the window
	EngineInfo::window->display();
}