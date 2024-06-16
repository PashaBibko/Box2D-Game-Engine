#pragma once

#include <util/util.h>

#include <engine/info.h>

// Foward declaration of engine classes
class PhysicalEntity;
class Entity;
class Engine;

/*
* @brief Class for body user data
*/
struct B2CustomUserData
{
	/*
	* @brief Embedded class for contact information
	*/
	struct ContatctInfo
	{
		Vec2 normal;
		Entity* collider;
	};

	// Pointer to the owner of the body
	Entity* owner;

	// Vector of contact information
	std::vector<ContatctInfo> contacts;

	// Current gravity on the body
	float gravityStrength = 0.0f;

	// If the body is grounded (colliding with the ground on a parallel horizontal line)
	bool grounded = false;
};

/*
* @brief Enum class for the type of entity
*/
enum class EntityType
{
	GRAPHIC_ONLY,
	GRAPHIC_PHYSICAL,

	UNDEFINED
};

/*
* @brief Base polymorphic class for all entities
*/
class Entity
{
	protected:
		// Allow the engine and world to access the private functions
		friend class World;
		friend class Engine;

		// Vector of all instances of Entity
		static std::vector<std::unique_ptr<Entity>> instances;

		/*
		* @brief Function called before b2World::Step is called
		*/
		virtual void preStepUpdate() {}

		/*
		* @brief Function called after b2World::Step is called
		*/
		virtual void postStepUpdate() {}

		// Type of entity
		EntityType type = EntityType::UNDEFINED;

		// Size of the entity
		Vec2 size;

		// Position of the entity
		Vec2 position;

		// Render states of the entity (set automatically)
		sf::RenderStates renderStates;

		//
		long long id;

	public:
		/*
		*/
		Entity();

		/*
		* @brief Virtual deconstructor to allow for polymorphism
		*/
		virtual ~Entity() {}

		/*
		* @brief Virtual function automatically called by the engine
		*/
		virtual void render() {}

		/*
		* @brief Getter for the size of the entity
		*/
		Vec2 getSize();

		/*
		* @brief Getter for the position of the entity
		* 
		* @return Position of the entity. Note the position of the entity on screen is modified by the scale
		*/
		Vec2 getPosition();

		/*
		* @brief Setter for the size of the entity
		* 
		* @param size New size of the entity
		*/
		void setSize(Vec2 size);

		/*
		* @brief Setter for the position of the entity
		* 
		* @param position New position of the entity
		*/
		void setPosition(Vec2 position);

		/*
		*/
		static void remove(Entity* entity);
};

/*
* @brief Base polymorphic class for all EngineControllers
*/
class EngineController
{
	private:
		// Allow the engine to access the private functions
		friend class Engine;

		/*
		* @brief Function called when the engine is created which calls the init function and any child controllers
		*/
		void onInit();

		/*
		* @brief Function called when the engine is rendered which calls the render function and any child controllers
		*/
		void onRender();

		/*
		* @brief Function called when the engine is updated which calls the update function and any child controllers
		*/
		void onUpdate();

	protected:
		// Static pointer to the engine
		static Engine* engineInstance;

	public:
		/*
		* @brief Default constructor. Should never be called
		*/
		EngineController() {}

		/*
		* @breif Default destructor. Virtual to allow for inheritance
		*/
		virtual ~EngineController() {}

		/*
		* @brief Virtual function called when the engine is initialized
		*/
		virtual void init() {}

		/*
		* @brief Virtual function called when the engine is rendered
		*/
		virtual void render() {}

		/*
		* @brief Virtual function called when the engine is updated
		*/
		virtual void update() {}

		// Child controller
		std::unique_ptr<EngineController> childController;
};

/*
* @brief Class for the engine
*/
class Engine
{
	private:
		// Count of the number of instances of the engine
		static size_t instanceCount;

		/*
		*/
		class ContactListener : public b2ContactListener
		{
			public:
				/*
				*/
				void BeginContact(b2Contact* contact) override;

				/*
				*/
				void EndContact(b2Contact* contact) override;

				/*
				*/
				void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
				
				/*
				*/
				void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
		};

		ContactListener contactListenerInstance;

	public:
		// Unique pointer to the engine controller
		std::unique_ptr<EngineController>controller;

		/*
		* @brief Constructor for the engine
		* 
		* @param windowSize Size of the window
		* @param controller Controller for the engine
		* @param fullscreen Whether the window should be fullscreen
		*/
		Engine(Vec2 windowSize, std::unique_ptr<EngineController>controller = nullptr, bool fullscreen = false);

		/*
		* @brief Destructor for the engine
		*/
		~Engine();

		/*
		* @brief Function to update the engine. Automatically calls the controller onUpdate function
		*/
		void update();

		/*
		* @brief Function to render the engine. Automatically calls the controller onRender function
		*/
		void render();

		/*
		* @brief Gets frames since held / frames held for
		*/
		long getInputInfo(sf::Keyboard::Key key);

		/*
		* @brief Gets frames since held / frames held for
		*/
		long getInputInfo(sf::Mouse::Button button);

		/*
		* @brief Function to check if a key is pressed
		*/
		bool isPressed(sf::Keyboard::Key key) { return getInputInfo(key) > 0; }

		/*
		* @brief Function to check if a mouse button is pressed
		*/
		bool isPressed(sf::Mouse::Button button) { return getInputInfo(button) > 0; }

		/*
		* @brief Function to check if a key is clicked (first frame pressed)
		*/
		bool isClicked(sf::Keyboard::Key key) { return getInputInfo(key) == 1; }

		/*
		* @brief Function to check if a mouse button is clicked (first frame pressed)
		*/
		bool isClicked(sf::Mouse::Button button) { return getInputInfo(button) == 1; }

		/*
		* @brief Function to add a keybord key to the engine
		* 
		* @param key Key to add
		*/
		void addInput(sf::Keyboard::Key key);

		/*
		* @brief Function to add a mouse button to the engine
		* 
		* @param button Mouse button to add
		*/
		void addInput(sf::Mouse::Button button);

		/*
		* @brief Empty function to avoid recursion errors
		*/
		void addInputs() {}

		/*
		* @brief Function to add multiple inputs to the engine
		*/
		template <typename FIRST_INPUT_TYPE, typename... Args>
		void addInputs(FIRST_INPUT_TYPE firstInput, Args... args)
		{
			// To whom it may concern:
			// Yes I know I should be putting in into the .cpp file
			// but whenever I do that I get a linker error (shown below):
			// 
			// 1> main.obj : error LNK2019: unresolved external symbol "public: void __thiscall Engine::addInputs<enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Mouse::Button>(enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Keyboard::Key,enum sf::Mouse::Button)" (??$addInputs@W4Key@Keyboard@sf@@W4123@W4123@W4123@W4Button@Mouse@3@@Engine@@QAEXW4Key@Keyboard@sf@@000W4Button@Mouse@3@@Z) referenced in function _main
			// 1> C:\Users\Pasha\source\scripts\SFML - Template\Debug\Box2D - Game - Engine.exe : fatal error LNK1120 : 1 unresolved externals
			// 1> Done building project "Box2D-Game-Engine.vcxproj" -- FAILED.

			// Checks the type of the first input is valid
			static_assert (
				std::is_same<FIRST_INPUT_TYPE, sf::Keyboard::Key>::value ||
				std::is_same<FIRST_INPUT_TYPE, sf::Mouse::Button>::value,
				"Invalid input type"
			);

			// Add the first input
			addInput(firstInput);

			// Recursively add the rest of the inputs
			addInputs(args...);
		}
};

/*
* @brief Macro for a simple loop of the engine
*/
#define BASIC_LOOP(instance) \
while (EngineInfo::windowOpen) \
{ \
	instance.update(); \
	instance.render(); \
}