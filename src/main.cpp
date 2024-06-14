#include <util/util.h>
#include <engine/engine.h>

class CustomController : public EngineController
{
	private:
		std::shared_ptr<Entity> entity;
		std::shared_ptr<Entity> object;

	public:
		void onInit() override
		{
			GraphicDef g_def;
			g_def.size = Vec2{2, 2};
			g_def.position = Vec2{2, 2};

			entity = GraphicEntity::create(g_def);

			PhysicalDef p_def;
			p_def.size = Vec2{2, 2};
			p_def.position = Vec2{8, 8};

			p_def.bodyType = b2_dynamicBody;

			p_def.fixtureVertices.resize(2);

			p_def.fixtureVertices[0] = {
				Vec2{0, 0},
				Vec2{2, 0},
				Vec2{2, 2},
				Vec2{0, 2}
			};

			p_def.fixtureVertices[1] = {
				Vec2(0, 0),
				Vec2(-3, 0),
				Vec2(-3, -3),
				Vec2(0, -3)
			};

			object = PhysicalEntity::create(p_def);
		}

		void onRender() override
		{
			entity->render();

			object->render();
		}

		void onUpdate() override
		{
			sf::View oldView = EngineInfo::window->getView();

			if (EngineInfo::keyMap[sf::Keyboard::Key::W] > 0)
				oldView.setCenter(oldView.getCenter().x, oldView.getCenter().y - 1);

			if (EngineInfo::keyMap[sf::Keyboard::Key::A] > 0)
				oldView.setCenter(oldView.getCenter().x - 1, oldView.getCenter().y);

			if (EngineInfo::keyMap[sf::Keyboard::Key::S] > 0)
				oldView.setCenter(oldView.getCenter().x, oldView.getCenter().y + 1);

			if (EngineInfo::keyMap[sf::Keyboard::Key::D] > 0)
				oldView.setCenter(oldView.getCenter().x + 1, oldView.getCenter().y);

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