#include <engine/base.h>

#include <util/util.h>

// Creates instances of static members of EngineSubClass

Engine* EngineSubClass::engineInstance = nullptr;

// Creates instances of static members of Engine

size_t Engine::instanceCount = 0;

const float Engine::pxToMeter = 50.0f;

std::unordered_map<sf::Keyboard::Key, long> Engine::keyMap;
std::unordered_map<sf::Mouse::Button, long> Engine::mouseMap;

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
	if (userDataA != nullptr && userDataB != nullptr)
	{
		info.collider = userDataB->owner;
		userDataA->contacts.push_back(info);

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

	// Sets the EngineSubClass::engineInstance to this
	EngineSubClass::engineInstance = this;

	// Throws an error if the instance count is not 1 and not allowed by the settings
	if (Engine::instanceCount != 1 && !ALLOW_MULTIPLE_INSTANCES)
		throw std::runtime_error("Multiple instances of the engine are not allowed");

	// Creates the window
	sf::VideoMode videoMode = (fullscreen) ? sf::VideoMode::getDesktopMode() : sf::VideoMode((int)windowSize.x, (int)windowSize.y);

	window.create(videoMode, "GAME ENGINE");
	window.setView(sf::View(sf::FloatRect(0, 0, windowSize.x, windowSize.y)));
	window.setFramerateLimit(60);

	windowRenderTexture.create(1920, 1080);

	// Creates the box2d world
	world = new b2World(Vec2(0.0f, 0.0f));
	world->SetContactListener(&contactListenerInstance);

	// Intialises the windowDisplayQuad
	windowDisplayQuad.setPrimitiveType(sf::TriangleStrip);
	windowDisplayQuad.resize(4);

	windowDisplayQuad[0].position = sf::Vector2f(0.0, 0.0);
	windowDisplayQuad[1].position = sf::Vector2f((float)window.getSize().x, 0.0);
	windowDisplayQuad[2].position = sf::Vector2f(0.0, (float)window.getSize().y);
	windowDisplayQuad[3].position = sf::Vector2f((float)window.getSize().x, (float)window.getSize().y);

	windowDisplayQuad[0].texCoords = sf::Vector2f(0, 0);
	windowDisplayQuad[1].texCoords = sf::Vector2f((float)windowRenderTexture.getSize().x, 0.0);
	windowDisplayQuad[2].texCoords = sf::Vector2f(0.0, (float)windowRenderTexture.getSize().y);
	windowDisplayQuad[3].texCoords = sf::Vector2f((float)windowRenderTexture.getSize().x, (float)windowRenderTexture.getSize().y);

	// I have no idea why this centres it onto the origin
	moveView({ -1920 / 3, -1080 / 3 });

	// Calls the init controller function if it exists
	if (this->controller != nullptr)
		this->controller->onInit();
}

Engine::~Engine()
{
	// Decrements the instance count
	Engine::instanceCount--;

	// Removes all entities
	while (Entity::instances.size() != 0)
		Entity::remove(Entity::instances[0].get());

	// Deletes the b2World
	delete world;
}

void Engine::update()
{
	// Polls window events

	sf::Event event;

	while (window.pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::MouseMoved:
				mousePos = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
				break;
		}
	}

	// Updates the keyMap and mouseMap

	#define MIDPOINT 0 // Can change (no logical reason why it should not be 0)

	callFuncOnMap(keyMap, [](sf::Keyboard::Key key, long& value) { sf::Keyboard::isKeyPressed(key) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });
	callFuncOnMap(mouseMap, [](sf::Mouse::Button button, long& value) { sf::Mouse::isButtonPressed(button) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });

	// Calls all Entity::preStepUpdate functions
	for (std::unique_ptr<Entity>& entity : Entity::instances)
		entity->preStepUpdate();

	// Updates the b2World
	world->Step(1.0f / 60.0f, 8, 3);

	// Calls the update functions of all entities
	for (std::unique_ptr<Entity>& entity : Entity::instances)
		entity->postStepUpdate();

	// Calls the update controller function if it exists
	if (controller != nullptr)
		controller->onUpdate();
}

void Engine::render()
{
	// Clears the window
	window.clear();

	// Clears the render texture
	windowRenderTexture.clear();

	// Calls the render controller function if it exists
	if (controller != nullptr)
		controller->onRender();

	// Displays the render texture
	windowRenderTexture.display();
	
	sf::RenderStates states;
	states.texture = &windowRenderTexture.getTexture();
	states.shader = &testShader;

	window.draw(windowDisplayQuad, states);

	// Displays the window
	window.display();
}

void Engine::moveView(Vec2 offset)
{
	sf::View view = windowRenderTexture.getView();
	view.move(offset.x, offset.y);
	windowRenderTexture.setView(view);
}

long Engine::getInputInfo(sf::Keyboard::Key key)
{
	// Checks key is in the keyMap
	if (keyMap.find(key) == keyMap.end())
		return 0;

	// Returns the value of the key in the keyMap
	return keyMap[key];
}

long Engine::getInputInfo(sf::Mouse::Button button)
{
	// Checks button is in the mouseMap
	if (mouseMap.find(button) == mouseMap.end())
		return 0;

	// Returns the value of the button in the mouseMap
	return mouseMap[button];
}

void Engine::addInput(sf::Keyboard::Key key)
{
	// Adds the key to the keyMap if it is not already in it
	if (!inMap(keyMap, key))
	{
		keyMap[key] = 0;
	}
}

void Engine::addInput(sf::Mouse::Button button)
{
	// Adds the button to the mouseMap if it is not already in it
	if (!inMap(mouseMap, button))
	{
		mouseMap[button] = 0;
	}
}