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

void EngineController::onClose()
{
	// Calls its onClose function
	close();

	// Calls the onClose function of the child controller if it exists
	if (childController != nullptr)
		childController->onClose();
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
		userDataA->contacts[userDataB->owner] = info;
		userDataB->contacts[userDataA->owner] = info;
	}
}

void Engine::ContactListener::EndContact(b2Contact* contact)
{
	GET_USER_DATA(contact);

	// Removes the contacts from the user data (if it can be found)
	if (userDataA != nullptr && userDataB != nullptr)
	{
		if (userDataA->owner == nullptr || userDataB->owner == nullptr)
			return;
		
		userDataA->contacts.erase(userDataB->owner);
		userDataB->contacts.erase(userDataA->owner);
	}
}

void Engine::ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	GET_USER_DATA(contact);

	// Checks both userData exsist
	if (userDataA != nullptr && userDataB != nullptr)
	{
		// Updates the normals of the contacts within the user data
		userDataA->contacts[userDataB->owner].normal = Vec2(contact->GetManifold()->localNormal);
		userDataB->contacts[userDataA->owner].normal = Vec2(contact->GetManifold()->localNormal);

		// Sets grounded state of the entity on top
		bool aOnTop = userDataA->owner->getPosition().y < userDataB->owner->getPosition().y;

		if (aOnTop)
			userDataA->grounded = true;

		else
			userDataB->grounded = true;
	}
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

	//
	engineClock.restart();

	// Calls the init controller function if it exists
	if (this->controller != nullptr)
		this->controller->onInit();
}

Engine::~Engine()
{
	// Calls the close controller function if it exists
	if (controller != nullptr)
		controller->onClose();

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
		}
	}

	// Updates the mouse position

	Vec2 pixelMousePos = sf::Mouse::getPosition(window);
	Vec2 windowMousePos = window.mapPixelToCoords(pixelMousePos);

	// Converts the mouse position to the position of the render texture

	Vec2 size = window.getSize();

	if (editorState != EditorState::INACTIVE)
		size.x -= 400;

	Vec2 renderTexturePixel;
	renderTexturePixel.x = windowMousePos.x / size.x * windowRenderTexture.getSize().x;
	renderTexturePixel.y = windowMousePos.y / size.y * windowRenderTexture.getSize().y;

	Vec2 renderTextureMousePos = windowRenderTexture.mapPixelToCoords(renderTexturePixel);
	mousePos = renderTextureMousePos;


	// Updates the keyMap and mouseMap

	#define MIDPOINT 0 // Can change (no logical reason why it should not be 0)

	// If LControl is not pressed, updates the keyMap and mouseMap
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
	{
		callFuncOnMap(keyMap, [](sf::Keyboard::Key key, long& value) { sf::Keyboard::isKeyPressed(key) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });
		callFuncOnMap(mouseMap, [](sf::Mouse::Button button, long& value) { sf::Mouse::isButtonPressed(button) ? ((value < MIDPOINT) ? value = MIDPOINT : value++) : ((value > MIDPOINT) ? value = MIDPOINT : value--); });
	}

	// Sets the editor state based on the number keys
	else
	{
		EditorState oldState = editorState;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0))
			editorState = EditorState::INACTIVE;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1))
			editorState = EditorState::EDITING;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2))
			editorState = EditorState::CREATING;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3))
			editorState = EditorState::MOVING;

		//

		if (oldState != editorState)
		{
			#define EDITOR_SIZE 400

			if (oldState == EditorState::INACTIVE && editorState != EditorState::INACTIVE)
			{
				Vec2 oldWindowSize = window.getSize();
				oldWindowSize.x = oldWindowSize.x + EDITOR_SIZE;
				window.setSize(oldWindowSize);

				sf::View oldView = window.getView();
				Vec2 oldViewSize = oldView.getSize();
				oldViewSize.x = oldViewSize.x + EDITOR_SIZE;
				oldView.setCenter(oldView.getCenter().x + EDITOR_SIZE / 2, oldView.getCenter().y);
				oldView.setSize(oldViewSize);
				window.setView(oldView);
			}

			if (oldState != EditorState::INACTIVE && editorState == EditorState::INACTIVE)
			{
				Vec2 oldWindowSize = window.getSize();
				oldWindowSize.x = oldWindowSize.x - EDITOR_SIZE;
				window.setSize(oldWindowSize);

				sf::View oldView = window.getView();
				Vec2 oldViewSize = oldView.getSize();
				Vec2 oldViewCenter = oldView.getCenter();
				oldViewSize.x = oldViewSize.x - EDITOR_SIZE;
				oldView.setCenter(oldViewCenter.x - EDITOR_SIZE / 2, oldViewCenter.y);
				oldView.setSize(oldViewSize);
				window.setView(oldView);
			}
		}

		if (editorState != EditorState::INACTIVE)
		{

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				if (editorSelectedEntity != nullptr)
					editorSelectedEntity->selectedByEditor = false;

				possibleEditorEntities.clear();

				Vec2 m = { mousePos.x / pxToMeter, mousePos.y / pxToMeter };

				possibleEditorEntities = QueryPoint(world, m);

				std::cout << "Possible Entities: " << possibleEditorEntities.size() << std::endl;
				std::cout << "Mouse Pos: " << m.x << ", " << m.y << std::endl;

				editorSelectedEntity = possibleEditorEntities.size() == 0 ? nullptr : possibleEditorEntities[0];

				if (editorSelectedEntity != nullptr)
					editorSelectedEntity->selectedByEditor = true;
			}
		}
	}

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
	window.clear(sf::Color::Black);

	// Clears the render texture
	windowRenderTexture.clear();

	// Calls the render controller function if it exists
	if (controller != nullptr)
		controller->onRender();

	// Displays the render texture
	windowRenderTexture.display();

	// Sets global shader uniforms

	testShader.setUniform("time", engineClock.getElapsedTime().asSeconds());
	testShader.setUniform("resolution", sf::Glsl::Vec2((float)window.getSize().x, (float)window.getSize().y));

	// Draws the render texture to the window
	sf::RenderStates states;
	states.texture = &windowRenderTexture.getTexture();
	states.shader = &testShader;

	window.draw(windowDisplayQuad, states);

	//
	if (editorState != EditorState::INACTIVE)
	{
		sf::RectangleShape divider;
		divider.setSize({ 8, (float)window.getSize().y });
		divider.setFillColor(sf::Color::Black);
		divider.setOutlineColor(sf::Color::White);
		divider.setOutlineThickness(1);
		divider.setPosition(windowDisplayQuad[1].position.x + 1, 0);

		window.draw(divider);

		//

		if (editorState != EditorState::INACTIVE)
		{
			sf::Font font;
			font.loadFromFile("C:/Users/Pasha/source/github-repos/Box2D-Game-Engine/res/fonts/BlockFont.ttf");

			sf::Text editorInfoText;
			editorInfoText.setFont(font);
			editorInfoText.setCharacterSize(24);
			editorInfoText.setFillColor(sf::Color::White);

			std::string editorInfoString = "Editor State: ";

			switch (editorState)
			{
			case EditorState::EDITING:
				editorInfoString += "Editing";
				break;

			case EditorState::CREATING:
				editorInfoString += "Creating";
				break;

			case EditorState::MOVING:
				editorInfoString += "Moving";
				break;
			}

			editorInfoString += "\nEntity Type: ";

			if (editorSelectedEntity == nullptr)
				editorInfoString += "None";

			else
			{
				if (editorSelectedEntity->type == EntityType::GRAPHIC_ONLY)
					editorInfoString += "Graphic";

				else if (editorSelectedEntity->type == EntityType::GRAPHIC_PHYSICAL)
					editorInfoString += "Physical";
			}

			editorInfoText.setString(editorInfoString);

			editorInfoText.setPosition(windowDisplayQuad[1].position.x + 20, 10);

			window.draw(editorInfoText);

			sf::RectangleShape editorInfoBoxDivider;
			editorInfoBoxDivider.setSize({ (float)window.getSize().x - windowDisplayQuad[1].position.x - 20, 8 });
			editorInfoBoxDivider.setFillColor(sf::Color::Black);
			editorInfoBoxDivider.setOutlineColor(sf::Color::White);
			editorInfoBoxDivider.setOutlineThickness(1);
			editorInfoBoxDivider.setPosition(windowDisplayQuad[1].position.x + 10, 70);

			window.draw(editorInfoBoxDivider);
		}

	}

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
	// Throws an error if the key is LControl (as it is used for the debug console)
	if (key == sf::Keyboard::Key::LControl)
		throw std::runtime_error("Cannot add LControl to keyMap");

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