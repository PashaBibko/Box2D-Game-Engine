#include <util/util.h>
#include <engine/engine.h>

class CustomController : public EngineController
{
	private:
		std::shared_ptr<Entity> object;

	public:
		void init() override
		{
			PhysicalDef p_def;
			p_def.size = Vec2{2, 2};
			p_def.position = Vec2{8, 8};

			p_def.fixtureVertices.resize(1);

			p_def.fixtureVertices[0] = {
				Vec2{-2, -2},
				Vec2{2, -2},
				Vec2{2, 2},
				Vec2{-2, 2}
			};

			object = PhysicalEntity::create(p_def);
		}

		void render() override
		{
			object->render();
		}

		void update() override
		{
			sf::View oldView = EngineInfo::window->getView();

			EngineInfo::window->setView(oldView);
		}
};

int main()
{
	Engine instance(Vec2{ 800, 600 }, std::make_unique<CustomController>());

	EngineInfo::keyMap = {
		{sf::Keyboard::Key::W, false},
		{sf::Keyboard::Key::A, false},
		{sf::Keyboard::Key::S, false},
		{sf::Keyboard::Key::D, false}
	};

	BASIC_LOOP(instance)

	return 0;
}