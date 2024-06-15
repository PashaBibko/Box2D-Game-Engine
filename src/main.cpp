#include <util/util.h>
#include <engine/engine.h>

class CustomController : public EngineController
{
	private:
		std::shared_ptr<Entity> platform;
		std::shared_ptr<Entity> player;

	public:
		void init() override
		{
			PhysicalDef p_def1;
			p_def1.size = Vec2{5, 1};
			p_def1.position = Vec2{8, 10};

			p_def1.fixtureVertices.resize(1);

			p_def1.fixtureVertices[0] = {
				Vec2{-5, -1},
				Vec2{5, -1},
				Vec2{5, 1},
				Vec2{-5, 1}
			};

			platform = PhysicalEntity::create(p_def1);

			//

			PhysicalDef p_def2;
			p_def2.size = Vec2{1, 1};
			p_def2.position = Vec2{8, 0};
			p_def2.bodyType = b2_dynamicBody;

			p_def2.fixtureVertices.resize(1);

			p_def2.fixtureVertices[0] = {
				Vec2{-1, -1},
				Vec2{1, -1},
				Vec2{1, 1},
				Vec2{-1, 1}
			};

			player = PhysicalEntity::create(p_def2);
		}

		void render() override
		{
			platform->render();
			player->render();
		}

		void update() override
		{
			sf::View view = EngineInfo::window->getView();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				view.move(1.0f, 0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				view.move(-1.0f, 0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				view.move(0, 1.0f);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				view.move(0, -1.0f);
			}

			EngineInfo::window->setView(view);

			PhysicalEntity* playerPtr = dynamic_cast<PhysicalEntity*>(player.get());

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && playerPtr->getB2UserData()->grounded)
			{
				playerPtr->setYVelocity(-20);
			}

			float newXVel = 0;
			newXVel = newXVel - (engineInstance->isPressed(sf::Keyboard::A) * 5);
			newXVel = newXVel + (engineInstance->isPressed(sf::Keyboard::D) * 5);

			playerPtr->setXVelocity(newXVel);
		}
};

int main()
{
	Engine instance(Vec2{ 800, 600 }, std::make_unique<CustomController>());

	instance.addInputs(
		sf::Keyboard::Left,
		sf::Keyboard::Right,
		sf::Keyboard::Up,
		sf::Keyboard::Down,

		sf::Keyboard::W,
		sf::Keyboard::A,
		sf::Keyboard::S,
		sf::Keyboard::D
	);

	BASIC_LOOP(instance);

	return 0;
}