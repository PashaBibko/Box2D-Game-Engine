#include <util/util.h>
#include <engine/engine.h>

#include <json.hpp>

long counter = 0;

class CustomController : public EngineController
{
	private:
		std::shared_ptr<Entity> platform1;
		std::shared_ptr<Entity> platform2;

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
			
			platform1 = PhysicalEntity::create(p_def1);

			p_def1.position = Vec2{8, 0};

			platform2 = PhysicalEntity::create(p_def1);

			//

			PhysicalDef p_def2;
			p_def2.size = Vec2{1, 1};
			p_def2.position = Vec2{8, 5};
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
			platform1->render();
			platform2->render();

			player->render();
		}

		void update() override
		{
			sf::View view = EngineInfo::window->getView();

			if (engineInstance->isPressed(sf::Keyboard::Left))
			{
				view.move(-2.0f, 0);
			}

			if (engineInstance->isPressed(sf::Keyboard::Right))
			{
				view.move(2.0f, 0);
			}

			if (engineInstance->isPressed(sf::Keyboard::Up))
			{
				view.move(0, -2.0f);
			}

			if (engineInstance->isPressed(sf::Keyboard::Down))
			{
				view.move(0, 2.0f);
			}

			EngineInfo::window->setView(view);

			//

			PhysicalEntity* playerPtr = dynamic_cast<PhysicalEntity*>(player.get());

			if (engineInstance->isPressed(sf::Keyboard::W) && playerPtr->getB2UserData()->grounded)
			{
				playerPtr->setYVelocity(-20);
			}

			float newXVel = 0;
			newXVel = newXVel - (engineInstance->isPressed(sf::Keyboard::A) * 5);
			newXVel = newXVel + (engineInstance->isPressed(sf::Keyboard::D) * 5);

			playerPtr->setXVelocity(newXVel);

			// THE CODE BELOW DOES NOT WORK
			// 
			// FOR SEMI-GOOD RESULTS USE engineInstance->isClicked(sf::Keyboard::R)
			// FOR FUNNY RESULTS USE     engineInstance->isPressed(sf::Keyboard::R)

			if (engineInstance->isPressed(sf::Keyboard::R))
			{
				PhysicalDef newPlayerDef;

				PhysicalEntity* playerPtr = dynamic_cast<PhysicalEntity*>(player.get());

				newPlayerDef = createDefOf(playerPtr);

				player = PhysicalEntity::create(newPlayerDef);

				counter++;
				std::cout << "Player recreated: " << counter << " \n";
			}
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
		sf::Keyboard::D,

		sf::Keyboard::R
	);

	BASIC_LOOP(instance);

	return 0;
}