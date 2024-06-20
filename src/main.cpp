#include <util/util.h>
#include <engine/engine.h>

#include <json.hpp>

class CustomController : public EngineController
{
	private:
		Entity* platform1 = nullptr;
		Entity* platform2 = nullptr;

		PhysicalEntity* player = nullptr;

	public:
		void init() override
		{
			PhysicalDef p_def1;
			p_def1.size = Vec2{400 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter };
			p_def1.position = Vec2{400 / engineInstance->pxToMeter, 500 / engineInstance->pxToMeter };

			p_def1.fixtureVertices.resize(1);

			p_def1.fixtureVertices[0] = {
				Vec2{-400 / engineInstance->pxToMeter, -50 / engineInstance->pxToMeter},
				Vec2{400 / engineInstance->pxToMeter, -50 / engineInstance->pxToMeter},
				Vec2{400 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter},
				Vec2{-400 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter}
			};
			
			platform1 = PhysicalEntity::create(p_def1);

			p_def1.position = Vec2{400 / engineInstance->pxToMeter, 0};

			platform2 = PhysicalEntity::create(p_def1);

			//

			PhysicalDef p_def2;
			p_def2.size = Vec2{50 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter };
			p_def2.position = Vec2{400 / engineInstance->pxToMeter, 250 / engineInstance->pxToMeter };
			p_def2.bodyType = b2_dynamicBody;

			p_def2.fixtureVertices.resize(1);

			p_def2.fixtureVertices[0] = {
				Vec2{-50 / engineInstance->pxToMeter, -50 / engineInstance->pxToMeter},
				Vec2{50 / engineInstance->pxToMeter, -50 / engineInstance->pxToMeter},
				Vec2{50 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter},
				Vec2{-50 / engineInstance->pxToMeter, 50 / engineInstance->pxToMeter}
			};

			player = PhysicalEntity::create(p_def2);

			//
		}

		void render() override
		{
			platform1->render();
			platform2->render();

			player->render();
		}

		void update() override
		{
			if (engineInstance->isPressed(sf::Keyboard::Left))
			{
				engineInstance->moveView({ -2.0f, 0 });
			}

			if (engineInstance->isPressed(sf::Keyboard::Right))
			{
				engineInstance->moveView({  2.0f, 0 });
			}

			if (engineInstance->isPressed(sf::Keyboard::Up))
			{
				engineInstance->moveView({ 0, -2.0f });
			}

			if (engineInstance->isPressed(sf::Keyboard::Down))
			{
				engineInstance->moveView({ 0,  2.0f });
			}

			//

			if (engineInstance->isPressed(sf::Keyboard::W) && player->getB2UserData()->grounded)
			{
				player->setYVelocity(-20);
			}

			float newXVel = 0;
			newXVel = newXVel - (engineInstance->isPressed(sf::Keyboard::A) * 5);
			newXVel = newXVel + (engineInstance->isPressed(sf::Keyboard::D) * 5);

			player->setXVelocity(newXVel);

			if (engineInstance->isClicked(sf::Keyboard::R))
			{
				PhysicalDef newPlayerDef;

				newPlayerDef = createDefOf(player);

				Entity::remove(player);

				player = PhysicalEntity::create(newPlayerDef);
			}
		}
};

int main()
{
	Engine instance(Vec2{ 1280, 720 }, std::make_unique<CustomController>());
	instance.testShader.loadFromFile("C:/Users/Pasha/source/github-repos/Box2D-Game-Engine/glsl/invert.frag", sf::Shader::Fragment);

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