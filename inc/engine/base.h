#pragma once

#include <util/util.h>

#include <engine/info.h>

// Foward declaration of the engine class
class Engine;

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
		// Allow the engine to access the private functions
		friend class Engine;

		// Vector of all instances of Entity
		static std::vector<std::shared_ptr<Entity>> instances;

		/*
		* @brief Virtual function automatically called by the engine
		*/
		virtual void update();

		// Type of entity
		EntityType type = EntityType::UNDEFINED;

		// Size of the entity
		Vec2 size;

		// Position of the entity
		Vec2 position;

		// Render states of the entity (set automatically)
		sf::RenderStates renderStates;

	public:
		/*
		* @brief Virtual function automatically called by the engine
		*/
		virtual void render();

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
		* @brief Function to update the engine. Automatically calls the controller onUpdate function
		*/
		void update();

		/*
		* @brief Function to render the engine. Automatically calls the controller onRender function
		*/
		void render();
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