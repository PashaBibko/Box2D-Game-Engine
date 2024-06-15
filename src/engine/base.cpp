#include <engine/base.h>

#include <util/util.h>

#include <engine/info.h>

// Creates instances of static members of EngineInfo

std::shared_ptr<sf::RenderWindow> EngineInfo::window = nullptr;
std::shared_ptr<b2World> EngineInfo::world = nullptr;

bool EngineInfo::windowOpen = false;

std::unordered_map<sf::Keyboard::Key, long> EngineInfo::keyMap;
std::unordered_map<sf::Mouse::Button, long> EngineInfo::mouseMap;

Vec2 EngineInfo::mousePos;

const float EngineInfo::scale = 50.0f;

#if DEBUG

bool EngineInfo::showHitboxes = false;

#endif

// Creates instances of static members of EngineController

Engine* EngineController::engineInstance = nullptr;

// Creates instances of static members of Engine

size_t Engine::instanceCount = 0;

// ----- EngineController Functions ----- //

void EngineController::onInit()
{
	// Calls its onInit function
	init();

	// Calls the onInit function of the child controller if it exists
	if (childController != nullptr)
		childController->onInit();
}

void EngineController::onUpdate()
{
	// Calls its onUpdate function
	update();

	// Calls the onUpdate function of the child controller if it exists
	if (childController != nullptr)
		childController->onUpdate();
}

void EngineController::onRender()
{
	// Calls its onRender function
	render();

	// Calls the onRender function of the child controller if it exists
	if (childController != nullptr)
		childController->onRender();
}

// ----- Engine Functions ----- //

Engine::Engine(Vec2 windowSize, std::unique_ptr<EngineController>controller, bool fullscreen) : controller(std::move(controller))
{
	// Increments the instance count
	Engine::instanceCount++;

	// Throws an error if the instance count is not 1 and not allowed by the settings
	if (Engine::instanceCount != 1 && !ALLOW_MULTIPLE_INSTANCES)
		throw std::runtime_error("Multiple instances of the engine are not allowed");

	// Creates the window
	sf::VideoMode videoMode = (fullscreen) ? sf::VideoMode::getDesktopMode() : sf::VideoMode((int)windowSize.x, (int)windowSize.y);

	EngineInfo::window = std::make_shared<sf::RenderWindow>(videoMode, "GAME ENGINE");
	EngineInfo::window->setView(sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y)));
	EngineInfo::window->setFramerateLimit(60);

	// Sets the windowOpen flag
	EngineInfo::windowOpen = EngineInfo::window->isOpen();

	// Creates the box2d world
	EngineInfo::world = std::make_shared<b2World>(Vec2(0.0f, 0.0f));
	EngineInfo::world->SetContactListener(&contactListenerInstance);

	// Sets the EngineController engine pointer to this
	EngineController::engineInstance = this;

	// Calls the init controller function if it exists
	if (this->controller != nullptr)
		this->controller->onInit();
}

Engine::~Engine()
{
	// Decrements the instance count
	Engine::instanceCount--;
}

void Engine::update()
{
	// Checks window and b2World are not nullptrs
	
	if (EngineInfo::window == nullptr || EngineInfo::world == nullptr)
		throw std::runtime_error("EngineInfo::window or EngineInfo::world is nullptr");

	// Polls window events

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

	// Updates the keyMap and mouseMap

	#define MIDPOINT 0 // Can change (no logical reason why it should not be 0)

	callFuncOnMap(EngineInfo::keyMap, [](sf::Keyboard::Key key, long& value) { sf::Keyboard::isKeyPressed(key) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });
	callFuncOnMap(EngineInfo::mouseMap, [](sf::Mouse::Button button, long& value) { sf::Mouse::isButtonPressed(button) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });

	// Calls all Entity::preStepUpdate functions
	for (std::shared_ptr<Entity> entity : Entity::instances)
		entity->preStepUpdate();

	// Updates the b2World
	EngineInfo::world->Step(1.0f / 60.0f, 8, 3);

	// Calls the update functions of all entities
	for (std::shared_ptr<Entity> entity : Entity::instances)
		entity->postStepUpdate();

	// Calls the update controller function if it exists
	if (controller != nullptr)
		controller->onUpdate();
}

void Engine::render()
{
	// Checks window is not nullptr
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

long Engine::getInputInfo(sf::Keyboard::Key key)
{
	// Checks key is in the keyMap
	if (EngineInfo::keyMap.find(key) == EngineInfo::keyMap.end())
		return 0;

	// Returns the value of the key in the keyMap
	return EngineInfo::keyMap[key];
}

long Engine::getInputInfo(sf::Mouse::Button button)
{
	// Checks button is in the mouseMap
	if (EngineInfo::mouseMap.find(button) == EngineInfo::mouseMap.end())
		return 0;

	// Returns the value of the button in the mouseMap
	return EngineInfo::mouseMap[button];
}

void Engine::addInput(sf::Keyboard::Key key)
{
	// Adds the key to the keyMap if it is not already in it
	if (!inMap(EngineInfo::keyMap, key))
	{
		EngineInfo::keyMap[key] = 0;
	}
}

void Engine::addInput(sf::Mouse::Button button)
{
	// Adds the button to the mouseMap if it is not already in it
	if (!inMap(EngineInfo::mouseMap, button))
	{
		EngineInfo::mouseMap[button] = 0;
	}
}