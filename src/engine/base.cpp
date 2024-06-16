#include <engine/base.h>

#include <util/util.h>

#include <engine/info.h>

// Creates instances of static members of EngineInfo

std::shared_ptr<sf::RenderWindow> EngineInfo::window = nullptr;
std::shared_ptr<b2World> EngineInfo::world = nullptr;

sf::Shader EngineInfo::globalShader;

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

// ----- Contact Listener Functions ----- //

#ifndef GET_USER_DATA
#define GET_USER_DATA(contact) \
B2CustomUserData* userDataA = reinterpret_cast<B2CustomUserData*>(contact->GetFixtureA()->GetBody()->GetUserData().pointer); \
B2CustomUserData* userDataB = reinterpret_cast<B2CustomUserData*>(contact->GetFixtureB()->GetBody()->GetUserData().pointer);
#else
#error GET_USER_DATA already defined
#endif

void Engine::ContactListener::BeginContact(b2Contact* contact)
{
	GET_USER_DATA(contact);

	// Adds the contact to the user data
	B2CustomUserData::ContatctInfo info;
	info.normal = Vec2(contact->GetManifold()->localNormal);

	// Adds the contact to the user data of A (if it exists)
	if (userDataA != nullptr)
	{
		info.collider = userDataB->owner;
		userDataA->contacts.push_back(info);
	}

	// Adds the contact to the user data of B (if it exists)
	if (userDataB != nullptr)
	{
		info.collider = userDataA->owner;
		userDataB->contacts.push_back(info);
	}
}

void Engine::ContactListener::EndContact(b2Contact* contact)
{
	GET_USER_DATA(contact);

	// Removes the contact from the user data (if it can be found)
	for (size_t i = 0; i < userDataA->contacts.size(); i++)
	{
		if (userDataA->contacts[i].collider == userDataB->owner)
		{
			userDataA->contacts.erase(userDataA->contacts.begin() + i);
			break;
		}
	}
	// Removes the contact from the user data (if it can be found)
	for (size_t i = 0; i < userDataB->contacts.size(); i++)
	{
		if (userDataB->contacts[i].collider == userDataA->owner)
		{
			userDataB->contacts.erase(userDataB->contacts.begin() + i);
			break;
		}
	}
}

void Engine::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	GET_USER_DATA(contact);

	// Updates the normals of the contacts within the user data for userDataA
	for (size_t i = 0; i < userDataA->contacts.size(); i++)
	{
		if (userDataA->contacts[i].collider == userDataB->owner)
		{
			userDataA->contacts[i].normal = Vec2(contact->GetManifold()->localNormal);
			break;
		}
	}

	// Updates the normals of the contacts within the user data for userDataB
	for (size_t i = 0; i < userDataB->contacts.size(); i++)
	{
		if (userDataB->contacts[i].collider == userDataA->owner)
		{
			userDataB->contacts[i].normal = Vec2(contact->GetManifold()->localNormal);
			break;
		}
	}

	// Sets grounded state of the entity on top
	bool aOnTop = userDataA->owner->getPosition().y < userDataB->owner->getPosition().y;

	if (aOnTop)
		userDataA->grounded = true;

	else
		userDataB->grounded = true;
}

void Engine::ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	GET_USER_DATA(contact);
}

#undef GET_USER_DATA

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

	//
	while (Entity::instances.size() != 0)
	{
		Entity::remove(Entity::instances[0].get());
	}
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
	for (std::unique_ptr<Entity>& entity : Entity::instances)
		entity->preStepUpdate();

	// Updates the b2World
	EngineInfo::world->Step(1.0f / 60.0f, 8, 3);

	// Calls the update functions of all entities
	for (std::unique_ptr<Entity>& entity : Entity::instances)
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